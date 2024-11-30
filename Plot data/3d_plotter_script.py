import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
from mpl_toolkits.mplot3d import Axes3D

# Load the CSV data into a DataFrame
data = pd.read_csv('plot5_aats_l1_l2.csv')  # Replace 'your_data.csv' with the actual file name

# Extract x, y, z values from the dataframe
x = data['LOG2_L1_size'].values
y = data['LOG2_L2_size'].values
z = data['aat'].values

# Reshape z into a grid format, assuming the data is structured properly
# First, find the unique x and y values to create a grid
x_unique = np.sort(data['LOG2_L1_size'].unique())
y_unique = np.sort(data['LOG2_L2_size'].unique())

# Now reshape z to match the grid size (rows corresponding to x, columns to y)
Z = z.reshape(len(x_unique), len(y_unique))
print(x_unique)
print(y_unique)
print(Z)
# Create a meshgrid for X and Y
X, Y = np.meshgrid(x_unique, y_unique)

min_index = np.argmin(Z)
print(min_index)
x_min = 12
y_min = 16
z_min = Z.flatten()[min_index]
print(f"Minimal point is at: x={x_min}, y={y_min}, z={z_min}")

# Create a figure for the plot
fig = plt.figure(figsize=(7, 5))
ax = fig.add_subplot(111, projection='3d')
ax.scatter(x_min, y_min, z_min, color='r', s=100, label=f'Minimal point (x={x_min:.2f}, y={y_min:.2f}, z={z_min:.2f})')

# Plot the surface
surf = ax.plot_surface(X, Y, Z.T, cmap='viridis', edgecolor='none')  # Transpose Z to match grid

# Add a color bar to represent the Z values
fig.colorbar(surf)

# Label axes
ax.set_xlabel('LOG2_L1_size')
ax.set_ylabel('LOG2_L2_size')
ax.set_zlabel('aat')

# Show plot
plt.show()
