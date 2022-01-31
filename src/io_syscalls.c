
int TtyWrite(int tty_id,
             void *buf,
             int len)
{
    // Write the contents of buf to the terminal tty_id
    // Block calling process until the buffer has been written down
    // On success return len, in case of error return ERROR
    // If len is greater than TERMINAL_MAX_LINE then do several calls
    // I don't know whether this should be taken care of in the handler
    // or here?
}

void TtyTransmit(int tty_id,
                 void *buf,
                 int len)
{
    
}