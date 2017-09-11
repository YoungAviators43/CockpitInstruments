
typedef enum { MPS_NONE, MPS_STATUS_REQ, MPS_IDENTIFY_REQ, MPS_UNKNOWN_REQ } MpsRequest_t;


void MessageProcessSpy_Decode(void);
MpsRequest_t MessageProcessSpy_LastCommand(void);

