import os


def merge_binary_files_in_folder(folder_path, output_file, chunk_size=1024):
    """
    Merges all binary files in a folder into a single output file.

    Args:
        folder_path (str): Path to the folder containing binary files.
        output_file (str): Path to the output binary file.
        chunk_size (int): Size of chunks to read and write at a time.
    """
    with open(output_file, 'wb') as out:
        print(sorted(os.listdir(folder_path)))
        for filename in sorted(os.listdir(folder_path)):
            file_path = os.path.join(folder_path, filename)

            # Check if it's a file
            if os.path.isfile(file_path):
                print(f"Merging file: {file_path}")
                with open(file_path, 'rb') as f:
                    while chunk := f.read(chunk_size):
                        out.write(chunk)

    print(f"All files merged into {output_file}")


def merge_scatter_binary_files_in_folder(folder_path, output_file, start, end, chunk_size=1024):
    """
    Merges binary files with numeric filenames in a given range into a single output file.

    Args:
        folder_path (str): Path to the folder containing binary files.
        output_file (str): Path to the output binary file.
        start (int): Minimum filename (inclusive) to include.
        end (int): Maximum filename (inclusive) to include.
        chunk_size (int): Size of chunks to read and write at a time.
    """
    with open(output_file, 'wb') as out:
        for filename in sorted(os.listdir(folder_path), key=lambda x: int(x)):
            try:
                file_num = int(filename)
            except ValueError:
                # Skip files with non-integer names
                print("Wrong file name found as " + filename)
                continue

            if start <= file_num <= end:
                file_path = os.path.join(folder_path, filename)
                if os.path.isfile(file_path):
                    print(f"Merging file: {file_path}")
                    with open(file_path, 'rb') as f:
                        while chunk := f.read(chunk_size):
                            out.write(chunk)

    print(f"Files from {start} to {end} merged into {output_file}")


def merge_binary_files_in_chunks(file1, file2, output_file, chunk_size=1024):
    with open(file1, 'rb') as f1, open(file2, 'rb') as f2, open(output_file, 'wb') as out:
        # Read and write file1 in chunks
        while chunk := f1.read(chunk_size):
            out.write(chunk)
        # Read and write file2 in chunks
        while chunk := f2.read(chunk_size):
            out.write(chunk)

if __name__ == "__main__":
    input_data_path = '/hdd/phase2_data2/run663_combine_2176V_1638mBar_square'
    output_data = '/hdd/phase2_data2/run663_combine_2176V_1638mBar_square.dat'
    merge_binary_files_in_folder(input_data_path,output_data)