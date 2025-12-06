import matplotlib.pyplot as plt
import re
import sys
import os

def parse_complex(text):
    # Matches (real,imag)
    match = re.match(r'\(([^,]+),([^)]+)\)', text)
    if match:
        return complex(float(match.group(1)), float(match.group(2)))
    return None

def main():
    filenames = sys.argv[1:]
    if not filenames:
        filenames = ['output.txt']
    
    # Limit to max 3 files
    filenames = filenames[:3]

    plt.figure(figsize=(10, 6))
    
    files_plotted = False
    for filename in filenames:
        if not os.path.exists(filename):
            print(f"Error: File {filename} not found.")
            continue

        data = []
        with open(filename, 'r') as f:
            for line in f:
                line = line.strip()
                if not line: continue
                c = parse_complex(line)
                if c is not None:
                    data.append(abs(c))
        
        if data:
            plt.plot(data, label=filename)
            files_plotted = True

    if not files_plotted:
        return

    plt.title('FFT Magnitude')
    plt.xlabel('Index')
    plt.ylabel('Magnitude')
    plt.grid(True)
    plt.legend()
    output_image = 'fft_plot.png'
    plt.savefig(output_image)
    print(f"Plot saved to {output_image}")
    print("To view the plot, open fft_plot.png")

if __name__ == '__main__':
    main()
