import subprocess
import sys
import re
import json
import os
from datetime import datetime
import matplotlib.pyplot as plt

def parse_output(output):
    """Parse la sortie pour extraire temps"""
    data = {}
    
    lines = output.strip().split('\n')
    for line in lines:
        # Chercher "Average CPU time for X : Y"
        match = re.search(r'Average CPU time for (\w) : ([\d.]+)', line)
        if match:
            algo = match.group(1)
            time = float(match.group(2))
            data[algo] = {'mean_time': time}
        
        # Chercher "X / Y satisfaisables"
        match = re.search(r'(\d+) / (\d+) satisfaisables', line)
        if match:
            sat = int(match.group(1))
            total = int(match.group(2))
            # Ajouter aux dernières données parsées
            for algo in data:
                if 'sat_count' not in data[algo]:
                    data[algo]['sat_count'] = sat
                    data[algo]['total_count'] = total
    
    return data

def run_benchmark(algos, database_path):
    """Exécute le benchmark pour les algos spécifiés"""
    results = {}
    
    print(f"\n{'='*60}")
    print(f"Benchmarking algorithmes: {algos}")
    print(f"Base de données: {database_path}")
    print(f"{'='*60}\n")
    
    cmd = ['./bin/compare', algos, database_path]
    
    try:
        output = subprocess.check_output(cmd, stderr=subprocess.STDOUT, text=True)
        print(output)
        
        results = parse_output(output)
        
        for algo, data in results.items():
            if data['mean_time'] is not None:
                print(f"✓ Algo '{algo}': {data['mean_time']:.6f} sec")
            else:
                print("mean_time is none")
        
    except subprocess.CalledProcessError as e:
        print(f"✗ Erreur: {e.output} 1")
    except Exception as e:
        print(f"✗ Erreur: {str(e)} 2")
    
    return results

def save_benchmark_data(results, database_path):
    """Sauvegarde les résultats en JSON"""
    
    # --- Créer dossier data/json s'il n'existe pas ---
    json_dir = "results/json"
    os.makedirs(json_dir, exist_ok=True)

    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    filename = f"benchmark_{timestamp}.json"
    filepath = os.path.join(json_dir, filename)

    data = {
        'timestamp': timestamp,
        'database': database_path,
        'results': results
    }

    with open(filepath, 'w') as f:
        json.dump(data, f, indent=2)

    print(f"\n✓ Données sauvegardées: {filepath}")
    return filepath

def plot_benchmark_results(results, database_path):
    """Crée un graphique comparatif avec temps en secondes"""

    # --- Créer dossier data/png s'il n'existe pas ---
    png_dir = "results/png"
    os.makedirs(png_dir, exist_ok=True)

    # Filtrer les algos valides
    algo_times = [(algo, data['mean_time']) for algo, data in results.items() if data.get('mean_time') is not None]

    if not algo_times:
        print("Aucune donnée à afficher")
        return

    # Trier par temps décroissant
    algo_times.sort(key=lambda x: x[1], reverse=True)
    algo_labels, times = zip(*algo_times)

    algo_names = {
        'n': 'Naïf',
        'd': 'DPLL',
        'p': 'PPSZ',
        's': 'Schöning',
        'u': 'Schöning+UP',
        'c': 'Picosat (CDCL)'
    }

    labels = [algo_names.get(a, a) for a in algo_labels]

    fig, ax = plt.subplots(figsize=(10, 6))
    bars = ax.bar(labels, times, color=['#FF6B6B', '#4ECDC4', '#45B7D1', '#FFA07A', '#98D8C8', '#F7DC6F'][:len(times)])

    ax.set_ylabel('Temps CPU (secondes)', fontsize=12)
    ax.set_title(f'Comparaison Algorithmes SAT\n{database_path}', fontsize=14, fontweight='bold')
    ax.set_ylim(bottom=0)

    for bar, t in zip(bars, times):
        ax.text(bar.get_x() + bar.get_width()/2., bar.get_height(),
                f'{t:.2e} s',
                ha='center', va='bottom', fontsize=9)

    plt.tight_layout()

    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    filename = f"benchmark_graph_{timestamp}.png"
    filepath = os.path.join(png_dir, filename)

    plt.savefig(filepath, dpi=300, bbox_inches='tight')
    print(f"✓ Graphique sauvegardé: {filepath}")

    plt.show()

def load_and_compare_benchmarks(json_files):
    """Compare plusieurs fichiers JSON"""
    all_data = {}
    
    for json_file in json_files:
        if not os.path.exists(json_file):
            print(f"Fichier non trouvé: {json_file}")
            continue
        
        with open(json_file, 'r') as f:
            data = json.load(f)
            label = f"{data.get('database', 'unknown')}\n{data.get('timestamp', '')}"
            all_data[label] = data['results']
    
    if not all_data:
        print("Aucune donnée trouvée")
        return
    
    # Graphique comparatif
    fig, ax = plt.subplots(figsize=(12, 7))
    
    algos = set()
    for results in all_data.values():
        algos.update(results.keys())
    
    algos = sorted(list(algos))
    x = range(len(algos))
    width = 0.8 / len(all_data)
    
    colors = ['#FF6B6B', '#4ECDC4', '#45B7D1', '#FFA07A', '#98D8C8', '#F7DC6F']
    
    for idx, (label, results) in enumerate(all_data.items()):
        times = [results.get(algo, {}).get('mean_time', 0) for algo in algos]
        offset = width * (idx - len(all_data)/2 + 0.5)
        ax.bar([xi + offset for xi in x], times, width, label=label, color=colors[idx % len(colors)])
    
    ax.set_xlabel('Algorithmes', fontsize=12)
    ax.set_ylabel('Temps CPU (secondes)', fontsize=12)
    ax.set_title('Comparaison Multi-Benchmarks', fontsize=14, fontweight='bold')
    ax.set_xticks(x)
    ax.set_xticklabels(algos)
    ax.legend(fontsize=9)
    
    plt.tight_layout()
    
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    filename = f"comparison_{timestamp}.png"
    plt.savefig(filename, dpi=300, bbox_inches='tight')
    print(f"✓ Comparaison sauvegardée: {filename}")
    
    plt.show()

if __name__ == '__main__':
    args = sys.argv[1:]
    
    if not args:
        print("Usage: python interface.py <command> [args]")
        print("\nCommandes:")
        print("  chrono <algo> <path>        - Benchmark simple")
        print("  compare <algos> <path>      - Benchmark multi-algo avec graphique")
        print("  plot <json1> [json2] ...    - Compare fichiers JSON")
        sys.exit(1)
    
    command = args[0]
    
    if command == "chrono":
        if len(args) < 3:
            print("Usage: python interface.py chrono <algo> <path>")
            sys.exit(1)
        subprocess.run(['./bin/chrono', args[1], args[2]])
    
    elif command == "compare":
        if len(args) < 3:
            print("Usage: python interface.py compare <algos> <path>")
            print("Exemple: python interface.py compare cp db/uf20-91")
            sys.exit(1)
        
        algos = args[1]
        path = args[2]
        
        results = run_benchmark(algos, path)
        save_benchmark_data(results, path)
        plot_benchmark_results(results, path)
    
    elif command == "plot":
        if len(args) < 2:
            print("Usage: python interface.py plot <json1> [json2] ...")
            sys.exit(1)
        
        load_and_compare_benchmarks(args[1:])
    
    else:
        # Ancien comportement (fallback)
        dir = "./bin/"
        match command:
            case "resolve":
                dir += "resolve"
                if len(args) == 2:
                    subprocess.run([dir, args[1]])
                elif len(args) == 3:
                    subprocess.run([dir, args[1], args[2]])
            case _:
                print(f"Commande inconnue: {command}")