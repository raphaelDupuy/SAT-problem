import subprocess
import sys

if __name__ == '__main__':

    dir = "./bin/timerInterface"

    args = sys.argv[1:]
    arguments = (dir, args[0], args[1], args[2])
    match (args[0]):
        case "chrono":
            pass
        case "compare":
            pass
        case "resolve":
            pass
        case _:
            pass

    result = subprocess.run(arguments).stdout
