
import csv

def main():
    with open("etc/interrupt-descs.txt") as f:
        read = csv.reader(f)
        for row in read:
            g = ""
            if row[2] == "No":
                g = "push 0"
            print("""
// {0}: {1}
isr{0}:
    cli
    {2}
    push {0}
    jmp isr_common_stub
""".format(row[0], row[1], g))

if __name__ == "__main__":
    main()
