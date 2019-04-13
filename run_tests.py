
import io
import subprocess
import time

PIPE = subprocess.PIPE

def run_nightingale(input_command, test_for):
    wait_for = "Nightingale shell"

    try:
        proc = subprocess.run(["ruby", "run.rb"],
                stdout=PIPE, stdin=PIPE, timeout=3)

        out, err = proc.communicate(input=input_command+'\n')
    except subprocess.TimeoutExpired:
        proc.kill()
        proc.wait()

    print(out)

def main():
    run_nightingale("test", "idk")

if __name__ == '__main__':
    main()

