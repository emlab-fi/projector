import sys
import numpy as np
from mayavi import mlab

def split_further(input_array, size):
    array = []
    for item in input_array:
        array.append(np.split(item, size))
    return np.array(array)


def main(data_path, size, data_index):
    x, y, z, d = np.loadtxt(data_path, delimiter=',', skiprows=1, unpack=True, usecols=(0,1,2, data_index+3))
    new_data = np.split(d, size)
    final_data = split_further(new_data, size)

    scalar = mlab.pipeline.scalar_field(np.swapaxes(final_data, 0, 2))
    mlab.pipeline.image_plane_widget(scalar, plane_orientation="x_axes", colormap="viridis")
    mlab.pipeline.image_plane_widget(scalar, plane_orientation="y_axes", colormap="viridis")
    mlab.pipeline.image_plane_widget(scalar, plane_orientation="z_axes", colormap="viridis")

    mlab.show()

if __name__ == "__main__":
    main(sys.argv[1], int(sys.argv[2]), int(sys.argv[3]))
