import os

for exp_name in os.listdir('.'):

    db = exp_name.split('_')[0]

    if db not in ('sqlite',):
        continue

    for iteration in os.listdir(exp_name):

        if iteration in ('experiment.log', 'results.sqlite'):
            continue

        if db == 'sqlite':
            output_file = os.path.join(exp_name, iteration, 'output.txt')

            if os.path.exists(output_file) and os.path.getsize(output_file) > 1000000:
                print(f'Removing {output_file}...')
                os.remove(output_file)
