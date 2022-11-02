import pandas as pd
import matplotlib.pyplot as plt

def plotLocations(myresults):

   df = pd.read_csv(myresults) # create a new dataframe
   print(df) # print the contents of the dataframe

   # A scatter plot
   ax = df.plot.scatter(x='PosX', y='PosY', alpha=0.5)

   for i, label in enumerate(df.ID):
      ax.annotate(label, (df.PosX.iat[i]+50, df.PosY.iat[i]-50))

   plt.title('Node positions')
   plt.xlabel('x [m]')
   plt.ylabel('y [m]')
   
   plt.tight_layout(pad=1.08) # default: 1.08
   plt.savefig('results_locations.pdf')

def plotResults(myresults):

   df = pd.read_csv(myresults) # create a new dataframe
   print(df) # print the contents of the dataframe

   # A bar plot
   ax = df.plot.bar(x='ID', y={'DataTX'}, rot=0) # plot something

   plt.title('My title')
   plt.xlabel('Node ID')
   plt.ylabel('Packets')
   
   plt.tight_layout(pad=1.08) # default: 1.08
   plt.savefig('results_packets.pdf')

def main():
   print('Hello!')

   results = '../src/results.csv'
   
   # Set to True to generate a plot
   generatePlot0 = True # plot0
   generatePlot1 = True # plot1

   if generatePlot0:
      print('Generating plot0')
      plotLocations(results)

   if generatePlot1:
      print('Generating plot1')
      plotResults(results)

if __name__ == '__main__':
    main()