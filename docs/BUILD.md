# Φ-SIG Build Instructions

## Dependencies

```bash
# Ubuntu/Debian
apt-get install -y build-essential libssl-dev liboqs-dev

# Or from source
git clone https://github.com/open-quantum-safe/liboqs.git
cd liboqs && mkdir build && cd build
cmake .. && make -j$(nproc) && make install
```

## Build Tests

```bash
# Core Keyless
gcc -O3 test_video1.c phi_sig.c -lssl -lcrypto -lm -o test1

# Post-Quantum
gcc -O3 test_video2.c phi_sig.c phi_sig_pq.c -loqs -lssl -lcrypto -lm -o test2

# Full Blown
gcc -O3 test_video3.c phi_sig.c phi_sig_pq.c -loqs -lssl -lcrypto -lm -o test3
```
