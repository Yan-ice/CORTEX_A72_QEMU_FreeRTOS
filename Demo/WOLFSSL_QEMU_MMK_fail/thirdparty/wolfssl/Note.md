./configure --prefix=/home/yanice/wolfssl-aarch64 \
CFLAGS="-DFREERTOS \
-I/home/yanice/Desktop/FreeRTOS/FreeRTOS/Source/include \
" --host=aarch64-linux-gnu --enable-singlethreaded \
--enable-sha --enable-aes --enable-cryptonly \
--enable-static --enable-ssh

