
import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_json('./app/Data Extraction/data/glacier1.json')

x_values = [items['subscriberCount'] for items in df["data"] ]
y_values = [items['viewCount'] for items in df["data"] ]


plt.plot(x_values, y_values, marker='o')
plt.xlabel('Subscribers')
plt.ylabel('Views')
plt.title('S vs V Over Time')

plt.show()

