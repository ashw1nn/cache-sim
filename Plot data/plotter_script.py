import numpy as np
import seaborn as sns
import pandas as pd
import matplotlib.pyplot as plt

# Create a column to identify the DataFrame source
# df1 = pd.read_csv("plot4_miss_rates_blocksize0.csv")
# df2 = pd.read_csv("plot4_miss_rates_blocksize1.csv")
# df3 = pd.read_csv("plot4_miss_rates_blocksize2.csv")
# df4 = pd.read_csv("plot4_miss_rates_blocksize3.csv")
# df5 = pd.read_csv("plot4_miss_rates_blocksize4.csv")
# df6 = pd.read_csv("plot4_miss_rates_blocksize5.csv")
df = pd.read_csv("plot7_first4.csv")
df2 = pd.read_csv("plot7_last2.csv")
print(df)
# df1['L1_size'] = '1KB'
# df2['L1_size'] = '2KB'
# df3['L1_size'] = '4KB'
# df4['L1_size'] = '8KB'
# df5['L1_size'] = '16KB'
# df6['L1_size'] = '32KB'

# Concatenate all DataFrames
# df_combined = pd.concat([df1, df2, df3, df4, df5, df6])

plt.figure(figsize=(7, 5))

# Plot using seaborn
sns.lineplot(data=df, x='l1_size', y='aat', hue='vc_num_block', palette=['#FF6347', '#4682B4', '#32CD32', '#FF00FF'], legend=False)
sns.lineplot(data=df2, x='l1_size', y='aat', hue='l1_assoc', palette=['#000000', '#FFD700'], legend=False)

# Creating a custom legend
custom_legend = [
    plt.Line2D([0], [0], color='#FF6347', lw=2, label='vc_num_block 0'),
    plt.Line2D([0], [0], color='#4682B4', lw=2, label='vc_num_block 4'),
    plt.Line2D([0], [0], color='#32CD32', lw=2, label='vc_num_block 8'),
    plt.Line2D([0], [0], color='#FF00FF', lw=2, label='vc_num_block 16'),
    plt.Line2D([0], [0], color='#000000', lw=2, label='l1_assoc 2'),
    plt.Line2D([0], [0], color='#FFD700', lw=2, label='l1_assoc 4')
]

# Adding the legend to the plot
plt.legend(handles=custom_legend, title="Legend")

# Adding labels and title
plt.xlabel('Log2(L1 Size) (B)')
plt.ylabel('AAT')
plt.title('AAT vs LOG2(L1 size)')
plt.savefig('q7_plot.png', dpi= 300, bbox_inches='tight')
plt.show()

