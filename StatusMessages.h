// StatusMessages.h
//
// These show all the format strings used by the status messages.
// Care must be taken if you change them because printf does not
// like having less arguments than it expects.

#define READ_PROBLEM "Warning: problem reading results in generation %d"

#define START_SIM "Running simulation..."

#define EVAL_IND "Evaluating individual %d of generation %d..."
#define STOP_RUN "Stopping run..."
#define INIT_RUN "Initializing run..."
#define EVAL_GEN "Evaluating generation %d"
#define EVAL_DONE "Evaluation complete"
#define SELECT_POINT "Selected point (%f, %f, %f)"
#define EVAL_SA_ITER "SA: Evaluating iteration %d"
#define EVAL_HC_ITER "HC: Evaluating iteration %d"