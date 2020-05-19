#include "mbed.h"
#include <cmath>
#include "DA7212.h"

#include "accelerometer_handler.h"
#include "config.h"
#include "magic_wand_model_data.h"

#include "tensorflow/lite/c/common.h"
#include "tensorflow/lite/micro/kernels/micro_ops.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/version.h"

#include "uLCD_4DGL.h"
#define bufferLength (32)
#define bufferLength (32)
#define signalLength (1024)
DA7212 audio;
int16_t waveform[kAudioTxBufferSize];
InterruptIn pause_b(SW2);
InterruptIn confirm_b(SW3);

Thread threadDNN(osPriorityNormal, 120*1024);
EventQueue queuethread(32 * EVENTS_EVENT_SIZE);

uLCD_4DGL uLCD(D1, D0, D2);
Serial pc(USBTX, USBRX);

int currentSong = 0;
int mode = 0; 
bool uLCD_cls = 0;
int song[120];
int noteLength[120];
int serialCount = 0;
int gesture_index;
char serialInBuffer[bufferLength];

void playNote(int freq){
    pc.printf("play = %d\r\n", freq);
  for (int i = 0; i < kAudioTxBufferSize; i++)
  {
    waveform[i] = (int16_t)(sin((double)i * 2. * M_PI / (double)(kAudioSampleFrequency / freq)) * ((1 << 16) - 1));
  }
  // the loop below will play the note for the duration of 1s
  
  for(int j = 0; j < kAudioSampleFrequency / kAudioTxBufferSize; ++j)
  {
    audio.spk.play(waveform, kAudioTxBufferSize);
  }
  
}

void uLCDprint(void){
  if(uLCD_cls){
    uLCD.cls();
    uLCD_cls = 0;
  }
  uLCD.locate(1, 1);
  uLCD.printf("mode = %d\n", mode);
  uLCD.printf("gesture = %d\n", gesture_index);
  if (currentSong == 0){
    uLCD.printf("\nLittle star\n");
  }else if (currentSong == 1){
   uLCD.printf("\nLittle  bee\n");
  }else if (currentSong == 2){
    uLCD.printf("\nJingle bell\n");
  }
  return;
}

// Return the result of the last prediction  Mechian Learning
int PredictGesture(float* output) {
  // How many times the most recent gesture has been matched in a row
  static int continuous_count = 0;
  // The result of the last prediction
  static int last_predict = -1;
  // Find whichever output has a probability > 0.8 (they sum to 1)
  int this_predict = -1;
  for (int i = 0; i < label_num; i++) {
    if (output[i] > 0.8) this_predict = i;
  }
  // No gesture was detected above the threshold
  if (this_predict == -1) {
    continuous_count = 0;
    last_predict = label_num;
    return label_num;
  }

  if (last_predict == this_predict) {
    continuous_count += 1;
  } else {
    continuous_count = 0;
  }
  last_predict = this_predict;

  // If we haven't yet had enough consecutive matches for this gesture,
  // report a negative result
  if (continuous_count < config.consecutiveInferenceThresholds[this_predict]) {
    return label_num;
  }
  // Otherwise, we've seen a positive result, so clear all our variables
  // and report it
  continuous_count = 0;
  last_predict = -1;
  return this_predict;
}

// main in lab7_model_deploy
void DNN() {
  // Create an area of memory to use for input, output, and intermediate arrays.
  // The size of this will depend on the model you're using, and may need to be
  // determined by experimentation.
  constexpr int kTensorArenaSize = 60 * 1024;
  uint8_t tensor_arena[kTensorArenaSize];

  // Whether we should clear the buffer next time we fetch data
  bool should_clear_buffer = false;
  bool got_data = false;

  // The gesture index of the prediction


  // Set up logging.
  static tflite::MicroErrorReporter micro_error_reporter;
  tflite::ErrorReporter* error_reporter = &micro_error_reporter;

  // Map the model into a usable data structure. This doesn't involve any
  // copying or parsing, it's a very lightweight operation.
  const tflite::Model* model = tflite::GetModel(g_magic_wand_model_data);
  if (model->version() != TFLITE_SCHEMA_VERSION) {
    error_reporter->Report(
        "Model provided is schema version %d not equal "
        "to supported version %d.",
        model->version(), TFLITE_SCHEMA_VERSION);
    return;
  }

  // Pull in only the operation implementations we need.
  // This relies on a complete list of all the ops needed by this graph.
  // An easier approach is to just use the AllOpsResolver, but this will
  // incur some penalty in code space for op implementations that are not
  // needed by this graph.
  static tflite::MicroOpResolver<6> micro_op_resolver;
  micro_op_resolver.AddBuiltin(
      tflite::BuiltinOperator_DEPTHWISE_CONV_2D,
      tflite::ops::micro::Register_DEPTHWISE_CONV_2D());
  micro_op_resolver.AddBuiltin(tflite::BuiltinOperator_MAX_POOL_2D,
                               tflite::ops::micro::Register_MAX_POOL_2D());
  micro_op_resolver.AddBuiltin(tflite::BuiltinOperator_CONV_2D,
                               tflite::ops::micro::Register_CONV_2D());
  micro_op_resolver.AddBuiltin(tflite::BuiltinOperator_FULLY_CONNECTED,
                               tflite::ops::micro::Register_FULLY_CONNECTED());
  micro_op_resolver.AddBuiltin(tflite::BuiltinOperator_SOFTMAX,
                               tflite::ops::micro::Register_SOFTMAX());
  micro_op_resolver.AddBuiltin(tflite::BuiltinOperator_RESHAPE,
                               tflite::ops::micro::Register_RESHAPE(), 1);

  // Build an interpreter to run the model with
  static tflite::MicroInterpreter static_interpreter(
      model, micro_op_resolver, tensor_arena, kTensorArenaSize, error_reporter);
  tflite::MicroInterpreter* interpreter = &static_interpreter;

  // Allocate memory from the tensor_arena for the model's tensors
  interpreter->AllocateTensors();

  // Obtain pointer to the model's input tensor
  TfLiteTensor* model_input = interpreter->input(0);
  if ((model_input->dims->size != 4) || (model_input->dims->data[0] != 1) ||
      (model_input->dims->data[1] != config.seq_length) ||
      (model_input->dims->data[2] != kChannelNumber) ||
      (model_input->type != kTfLiteFloat32)) {
    error_reporter->Report("Bad input tensor parameters in model");
    return;
  }

  int input_length = model_input->bytes / sizeof(float);

  TfLiteStatus setup_status = SetupAccelerometer(error_reporter);
  if (setup_status != kTfLiteOk) {
    error_reporter->Report("Set up failed\n");
    return;
  }

  error_reporter->Report("Set up successful...\n");

  while (true) {
    // Attempt to read new data from the accelerometer
    got_data = ReadAccelerometer(error_reporter, model_input->data.f,
                                 input_length, should_clear_buffer);

    // If there was no new data,
    // don't try to clear the buffer again and wait until next time
    if (!got_data) {
      should_clear_buffer = false;
      continue;
    }

    // Run inference, and report any error
    TfLiteStatus invoke_status = interpreter->Invoke();
    if (invoke_status != kTfLiteOk) {
      error_reporter->Report("Invoke failed on index: %d\n", begin_index);
      continue;
    }

    // Analyze the results to obtain a prediction
    gesture_index = PredictGesture(interpreter->output(0)->data.f);

    // Clear the buffer next time we read data
    should_clear_buffer = gesture_index < label_num;

    // Produce an output
    if (gesture_index < label_num) {
      error_reporter->Report(config.output_message[gesture_index]);
      if (mode == 1){
        if (gesture_index == 0){
          if (currentSong == 2){
            currentSong = 0;
          }else{
            currentSong++;
          }
        }else if (gesture_index == 2){
          if (currentSong == 0){
            currentSong = 2;
          }else{
            currentSong--;
          }
        }else if (gesture_index == 1){
          mode = 2;
          uLCD_cls = 1;
        }
      }else if (mode == 2){
        if (gesture_index == 0 || gesture_index == 1 || gesture_index == 2){
          currentSong = gesture_index;
        }
      }
    }
  }
}
DigitalOut green_led(LED2);
void loadSignal(void)
{
  green_led = 0;
  int i = 0, j = 0;
  serialCount = 0;
  audio.spk.pause();
  uLCD.locate(1, 1);
  while(i < 120)
  {
    if(pc.readable())
    {
      serialInBuffer[serialCount] = pc.getc();
      serialCount++;
      if(serialCount == 5)
      {
        serialInBuffer[serialCount] = '\0';
        song[i] = (float) atof(serialInBuffer) * 1000;
        pc.printf("i = %d, %d\r\n", i, song[i]);
        serialCount = 0;
        i++;
      }
    }
  }
  while(j < 120)
  {
    if(pc.readable())
    {
      serialInBuffer[serialCount] = pc.getc();
      serialCount++;
      if(serialCount == 5)
      {
        serialInBuffer[serialCount] = '\0';
        noteLength[j] = (float) atof(serialInBuffer) * 1000;
        pc.printf("j = %d, %d\r\n", j, noteLength[j]);
        serialCount = 0;
        j++;
      }
    }
  }
  green_led = 1;
  return;
}

void mode_select(){
  mode = 1;
}
void confirm(){
  mode = 0;
  uLCD_cls = 1;
}
void playMusic(){
      for (int i = 0; i < 40; i++){
        if(mode == 0){
          uLCD.cls();
          uLCD.locate(1, 1);
          uLCD.printf("mode = %d\n", mode);
          uLCD.printf("gesture = %d\n", gesture_index);
          if (currentSong == 0){
            uLCD.printf("\nLittle star\n");
          }else if (currentSong == 1){
          uLCD.printf("\nLittle  bee\n");
          }else if (currentSong == 2){
            uLCD.printf("\nJingle bell\n");
          }
          
          uLCD.printf("Playing...\n");
          int len = noteLength[40*currentSong + i];
          while(len--){
            playNote(song[40 * currentSong + i]);
            if(len < 1){
              wait(1.0);
            }
          }
        }
      }
}
int main(int argc, char* argv[]) {
  threadDNN.start(DNN);
  pause_b.rise(mode_select);
  confirm_b.rise(confirm);
  uLCD.printf("\nloading\n");
  loadSignal();
  uLCD.printf("\nfinish\n");
  wait(5);
  uLCD.cls();

  while(true){
    uLCDprint();
    playMusic();
  }
}