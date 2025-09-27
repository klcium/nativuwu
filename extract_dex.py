import struct
import os
import argparse

## This was french-gpt coded. Not my work.


def extract_dex_files(binary_file, output_dir):
    # Vérifie si le répertoire de sortie existe, sinon le crée
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)

    with open(binary_file, 'rb') as f:
        data = f.read()

    # Cherche les fichiers DEX dans le binaire
    dex_magic = b'dex\n'  # Magique DEX
    offset = 0
    dex_count = 0

    while True:
        # Trouve l'index du prochain fichier DEX
        index = data.find(dex_magic, offset)
        if index == -1:
            break  # Aucun autre fichier DEX trouvé

        # Lit la taille du fichier DEX
        dex_file_size = struct.unpack('I', data[index + 0x20:index + 0x24])[0]

        # Extrait le fichier DEX
        dex_file_data = data[index:index + dex_file_size]

        # Crée un nom de fichier pour le DEX extrait avec offset et taille
        hex_offset = hex(index)[2:]  # Convertit l'offset en hexadécimal
        dex_file_name = f'dex_file_{hex_offset}_end_{hex(index + dex_file_size)[2:]}.dex'
        dex_file_path = os.path.join(output_dir, dex_file_name)

        # Écrit le fichier DEX extrait
        with open(dex_file_path, 'wb') as dex_file:
            dex_file.write(dex_file_data)

        # Met à jour l'offset pour continuer la recherche
        offset = index + 1
        dex_count += 1

    print(f"Extraction terminée. {dex_count} fichiers DEX extraits dans '{output_dir}'.")

if __name__ == "__main__":
    # Configuration de l'argument parser
    parser = argparse.ArgumentParser(description='Extraire les fichiers DEX d\'un fichier binaire.')
    parser.add_argument('binary_file', type=str, help='Chemin du fichier binaire à parser.')
    parser.add_argument('output_dir', type=str, help='Répertoire de sortie pour les fichiers DEX.')

    # Analyse des arguments
    args = parser.parse_args()

    # Appel de la fonction d'extraction
    extract_dex_files(args.binary_file, args.output_dir)


