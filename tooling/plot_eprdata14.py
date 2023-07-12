import click
import math
import os
import matplotlib.pyplot as plt


def read_from_file(file_path, line_start):
    with open(file_path, 'r') as data_file:
        lines = data_file.readlines()
        header = lines[line_start - 1 : line_start + 11]
        data_count = int(header[6].split()[0])
        data_line_count = math.ceil(data_count / 4)
        data = lines[line_start + 11 : line_start + 11 + data_line_count]
        del lines
    return header, data


def convert_data(data):
    output_data = []
    for line in data:
        for num in line.split():
            output_data.append(float(num))
    return output_data

def unlog_value(val):
    if (val == 0.0):
        return 0.0
    return math.exp(val)


def read_ESZG_block(data, N_y):
    energies = list(map(unlog_value, data[:N_y]))
    inc = list(map(unlog_value, data[N_y : 2 * N_y]))
    coh = list(map(unlog_value, data[2 * N_y : 3 * N_y]))
    pho_el = list(map(unlog_value, data[3 * N_y : 4 * N_y]))
    pair_prod = list(map(unlog_value, data[4 * N_y : 5 * N_y]))

    return energies, inc, coh, pho_el, pair_prod


def read_form_factors(data, JXS_2, N_inc, JXS_3, N_coh):
    inc_ff_var = data[JXS_2 - 1 : JXS_2 - 1 + N_inc]
    inc_ff = data[JXS_2 - 1 + N_inc : JXS_2 - 1 + 2 * N_inc]

    coh_ff_var = data[JXS_3 - 1 : JXS_3 - 1 + N_coh]
    coh_ff_cum = data[JXS_3 - 1 + N_coh : JXS_3 - 1 + 2 * N_coh]
    coh_ff_diff = data[JXS_3 - 1 + 2 * N_coh : JXS_3 - 1 + 3 * N_coh]

    return inc_ff_var, inc_ff, coh_ff_var, coh_ff_cum, coh_ff_diff

def parse_data(header, data):
    converted_data = convert_data(data)

    total_data = int(header[6].split()[0])

    assert(total_data == len(converted_data))

    N_y = int(header[6].split()[2])

    JXS_2 = int(header[8].split()[1])
    JXS_3 = int(header[8].split()[2])
    JXS_4 = int(header[8].split()[3])

    N_inc = (JXS_3 - JXS_2) // 2
    N_coh = (JXS_4 - JXS_3) // 3

    ESZG_data = read_ESZG_block(converted_data, N_y)
    form_factors = read_form_factors(converted_data, JXS_2, N_inc, JXS_3, N_coh)

    return ESZG_data, form_factors



def plot_data(parsed_data):
    ESZG_data = parsed_data[0]
    form_factors = parsed_data[1]

    fig, (ax1, ax2) = plt.subplots(1, 2, sharex=True, sharey=True)

    ax1.plot(ESZG_data[0], ESZG_data[1], label='incoherent')
    ax1.plot(ESZG_data[0], ESZG_data[2], label='coherent')
    ax1.plot(ESZG_data[0], ESZG_data[3], label='photoeletric')
    ax1.plot(ESZG_data[0], ESZG_data[4], label='pair production')
    ax1.set_title("Cross sections")
    ax1.grid(color='k', linestyle='--', linewidth=0.5)
    ax1.legend()
    ax1.set_xlabel("energy (MeV)")
    ax1.set_ylabel("barns/atom")
    ax1.set_yscale('log')
    ax1.set_xscale('log')

    ax2.plot(form_factors[0], form_factors[1], label='incoherent')
    ax2.plot(form_factors[2], form_factors[3], label='cumulative coherent')
    ax2.plot(form_factors[2], form_factors[4], label='differential coherent')
    ax2.set_title("Form factors")
    ax2.grid(color='k', linestyle='--', linewidth=0.5)
    ax2.legend()
    ax2.set_xlabel("energy (MeV)")
    ax2.set_ylabel("value")
    ax2.set_yscale('log')
    ax2.set_xscale('log')
    plt.show()
    return



@click.command()
@click.option('-e', '--element', 'elem', help="Element to plot", required=True, type=int)
@click.argument('xsdir_path', type=click.Path(exists=True, dir_okay=False, resolve_path=True))
def main(xsdir_path, elem):
    if (elem < 1 or elem > 100):
        print("Invalid element")
        exit()

    base_path = os.path.split(xsdir_path)[0]

    print("Reading xsdir file:", xsdir_path)

    with open(xsdir_path, 'r') as xsdir_file:
        xsdir_lines = xsdir_file.readlines()

    element_data = xsdir_lines[elem - 1].split()

    data_file = os.path.join(base_path, element_data[2])

    print("atomic number:", elem)
    print("atomic weight ration:", element_data[1])
    print("data file:", data_file)
    print("data start:", element_data[5])
    print("data count:", element_data[6])


    header, data = read_from_file(data_file, int(element_data[5]))
    parsed = parse_data(header, data)
    plot_data(parsed)



if __name__ == "__main__":
    main()
