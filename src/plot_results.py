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
    
    # Define styles to distinguish overlapping plots
    linestyles = ['-', '--', ':', '-.']
    linewidths = [2.5, 2.0, 1.5]
    
    files_plotted = False
    for i, filename in enumerate(filenames):
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
            # Cycle through styles if more than defined
            ls = linestyles[i % len(linestyles)]
            lw = linewidths[i % len(linewidths)]
            plt.plot(data, label=filename, linestyle=ls, linewidth=lw, alpha=0.8)
            files_plotted = True

    if not files_plotted:
        return

    plt.title('FFT Magnitude')
    plt.xlabel('Index')
    plt.ylabel('Magnitude')
    plt.grid(True)
    plt.legend()
    base_name = os.path.splitext(os.path.basename(filename))[0]
    output_image = f"{base_name}_plot.png"
    plt.savefig(output_image)
    print(f"Plot saved to {output_image}")
    print(f"To view the plot, open {output_image}")

if __name__ == '__main__':
    main()
