import click
import matplotlib.pyplot as plt


@click.command()
@click.argument('input_file', type=click.Path(exists=True, dir_okay=False, resolve_path=True))
def main(input_file):
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

    fig = plt.figure()
    ax = fig.add_subplot(projection='3d')

    ax.plot(x, y, z, c = 'b')
    ax.set_box_aspect([1, 1, 1])
    plt.show()



if __name__ == "__main__":
    main()