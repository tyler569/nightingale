;; Nightingale-aarch64

.globl start
start:

.loop:
  wfe
  b .loop
