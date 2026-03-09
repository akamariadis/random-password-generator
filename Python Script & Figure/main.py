import matplotlib.pyplot as plt
import numpy as np

def create_bias_figure():
    rng_max = 10
    target_n = 6
    raw_inputs = np.arange(rng_max + 1)
    modulo_results = raw_inputs % target_n
    mod_counts = np.zeros(target_n)
    for res in modulo_results:
        mod_counts[res] += 1
    limit = (len(raw_inputs) // target_n) * target_n
    rejection_results = []
    rejected_count = 0
    for r in raw_inputs:
        if r < limit:
            rejection_results.append(r % target_n)
        else:
            rejected_count += 1

    rej_counts = np.zeros(target_n)
    for res in rejection_results:
        rej_counts[res] += 1
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(12, 6), sharey=True)
    plt.subplots_adjust(top=0.85, wspace=0.3)
    color_bias = '#d9534f'
    color_safe = '#5cb85c'
    color_neutral = '#4e8cff'
    bars1 = ax1.bar(range(target_n), mod_counts, color=color_neutral, edgecolor='black', alpha=0.7)
    bars1[5].set_color(color_bias)
    bars1[5].set_edgecolor('black')
    ax1.set_title('A. Naive Modulo Bias (r % n)', fontsize=14, fontweight='bold')
    ax1.set_xlabel('Mapped Value', fontsize=12)
    ax1.set_ylabel('Probability (Frequency)', fontsize=12)
    ax1.set_xticks(range(target_n))
    ax1.grid(axis='y', linestyle='--', alpha=0.3)
    ax1.text(0.5, 0.95, f'Source Range: 0-{rng_max}\nTarget Range: 0-{target_n - 1}',
             transform=ax1.transAxes, ha='center', va='top',
             bbox=dict(boxstyle="round,pad=0.3", fc="white", ec="gray", alpha=0.9))
    bars2 = ax2.bar(range(target_n), rej_counts, color=color_safe, edgecolor='black', alpha=0.7)
    ax2.set_title('B. Rejection Sampling (Secure)', fontsize=14, fontweight='bold')
    ax2.set_xlabel('Mapped Value', fontsize=12)
    ax2.set_xticks(range(target_n))
    ax2.grid(axis='y', linestyle='--', alpha=0.3)
    ax2.text(0.5, 0.95, f'Limit Calculated: {limit}\n(Values {limit}-{rng_max} Discarded)',
             transform=ax2.transAxes, ha='center', va='top',
             bbox=dict(boxstyle="round,pad=0.3", fc="white", ec="gray", alpha=0.9))
    fig.suptitle('Impact of Mapping Algorithms on Distribution Uniformity', fontsize=16)
    plt.savefig('modulo_bias_figure.png', dpi=300, bbox_inches='tight')
    plt.show()
    print("Figure saved as 'modulo_bias_figure.png'")

if __name__ == "__main__":
    create_bias_figure()