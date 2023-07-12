# Lock-free Container Example

std::atomic을 활용하여 CAS(Check-and-Swap)를 수행하는 Lock-free Queue와 Stack.

싱글/멀티 스레드 환경에서 사용 가능하고 push/pop 과정에 lock을 거는 것 보다는 괜찮지 않을까 싶음.
성능을 측정해보진 않아서 항상 좋을지에 대한 것은 명확히 모르겠음.
