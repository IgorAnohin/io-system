import os
import shutil
import itertools as it
import subprocess

def perform_speed_by_dd_command(dd_if: str, dd_of: str) -> float:
    command = f"dd if={dd_if} of={dd_of}"
    out = subprocess.run(command, stderr=subprocess.PIPE, shell=True)
    stderr = out.stderr.decode()
    stats_line = stderr.strip().split("\n")[-1]
    speed, units = stats_line.split()[-2:]
    return float(speed.replace(",", "."))


def benchmark(src_dir, dst_dir, file):
    shutil.copy(file, src_dir)

    src = f"{src_dir}/{os.path.basename(file)}"
    dst = f"{dst_dir}/slave"

    tests = []
    for _ in range(30):
        speed = perform_speed_by_dd_command(src, dst)
        tests.append(speed)

    avg_speed = sum(tests) / len(tests)
    print(f"From {src_dir} to {dst_dir}, speed={avg_speed} Mb/s")

    os.remove(src)
    os.remove(dst)


if __name__ == "__main__":
    partitions = os.getenv("PARTITIONS").split()
    print("partitions:", partitions)

    benchmark_file = "/tmp/file.benchmark"
    paths = [f"/mnt/{p}" for p in partitions]
    paths.append(os.environ["HOME"])
    for p1, p2 in it.permutations(paths, 2):
        benchmark(p1, p2, benchmark_file)
