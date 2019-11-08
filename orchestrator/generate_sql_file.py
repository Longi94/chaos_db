import os
import argparse

parser = argparse.ArgumentParser()
parser.add_argument('-i', '--input', type=str, required=True)
parser.add_argument('-o', '--output', type=str, required=True)
args = parser.parse_args()

with open(args.input, 'r') as f:
    data = f.read()
    data = data.replace('{DIR}', os.getcwd())

with open(args.output, 'w') as f:
    f.write(data)
