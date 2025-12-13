import argparse
from pathlib import Path
from typing import Optional

import librosa
import numpy as np


def convert_m4a_to_txt(input_path: Path, output_path: Path, seconds: Optional[float] = None) -> None:
	"""Carica un file audio M4A e salva i campioni in un file di testo."""

	if not input_path.is_file():
		raise FileNotFoundError(f"File di input non trovato: {input_path}")

	y, sr = librosa.load(str(input_path), sr=None, mono=True)

	if seconds is not None:
		n_samples = int(sr * seconds)
		y = y[:n_samples]

	np.savetxt(output_path, y, fmt="%.6f")

	print(f"Fatto! Salvati {len(y)} campioni in '{output_path}'.")


def main() -> None:
	parser = argparse.ArgumentParser(description="Converte un file .m4a in un file .txt con i campioni audio.")
	parser.add_argument("input", help="File audio di input (.m4a)")
	parser.add_argument(
		"-o",
		"--output",
		help="File di output .txt (default: stesso nome con estensione .txt)",
		default=None,
	)
	parser.add_argument(
		"-s",
		"--seconds",
		type=float,
		default=None,
		help="Numero di secondi da convertire (default: tutto il file)",
	)

	args = parser.parse_args()

	input_path = Path(args.input).expanduser().resolve()
	if args.output is not None:
		output_path = Path(args.output).expanduser().resolve()
	else:
		output_path = input_path.with_suffix(".txt")

	convert_m4a_to_txt(input_path, output_path, args.seconds)


if __name__ == "__main__":
	main()