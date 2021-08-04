trait Socket {
    fn send(&self, &[u8]) -> core::result::Result<(), ()>;
    fn pending_receive(&self) -> bool;
    fn receive(&mut self) -> core::result::Result<(), ()>;
}

struct LoopbackSocket {
    data: Vec<u8>,
}

impl Socket for LoopbackSocket {

}
