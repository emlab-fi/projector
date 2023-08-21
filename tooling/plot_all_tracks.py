import click
import plotly.express as px
import pandas as pd
import glob
import os.path



def load_file(input_file):
    return pd.read_csv(input_file)

@click.command()
@click.argument('input_dir', type=click.Path(exists=True, file_okay=False, resolve_path=True))
def main(input_dir):


    files = glob.glob("photon*.csv", root_dir=input_dir)

    dataframes = []

    for file in files:
        df = load_file(os.path.join(input_dir, file))
        df["filename"] = file
        dataframes.append(df)

    final_data = pd.concat(dataframes)


    fig = px.line_3d(final_data, x="x", y="y", z="z", color="filename")
    fig.write_html("output.html")



if __name__ == "__main__":
    main()