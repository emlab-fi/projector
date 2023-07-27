import click
import matplotlib.pyplot as plt
import glob
import os.path



def load_file(input_file):
    with open(input_file) as file:
        lines = file.readlines()[1:]

    x = []
    y = []
    z = []

    for line in lines:
        line_split = line.split(",")
        x.append(float(line_split[0]))
        y.append(float(line_split[1]))
        z.append(float(line_split[2]))

    return x,y,z


@click.command()
@click.argument('input_dir', type=click.Path(exists=True, file_okay=False, resolve_path=True))
def main(input_dir):

    files = glob.glob("photon*.csv", root_dir=input_dir)

    fig = plt.figure()
    ax = fig.add_subplot(projection='3d')

    for file in files:
        x,y,z = load_file(os.path.join(input_dir, file))
        ax.plot(x, y, z, c = 'b', alpha=0.2)

    ax.set_box_aspect([1, 1, 1])
    plt.show()



if __name__ == "__main__":
    main()