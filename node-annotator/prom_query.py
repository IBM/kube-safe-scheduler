import csv
import requests
import time
import pandas as pd
import numpy as np
from statsmodels.tsa.arima_model import ARIMA
from sklearn.metrics import mean_squared_error


class FetchData:

    now = int(time.time())
    prom_address = 'http://127.0.0.1:9090'
    query = 'node_memory_MemFree{instance="127.0.0.1:9100"}'
    start = now - 3600
    end = now
    step = '15s'
    file_path = 'mem_usage.csv'
    data = pd.DataFrame()

    def __init__(self, my_prom_address, my_query):
        FetchData.prom_address = my_prom_address
        FetchData.query = my_query
        FetchData.now = int(time.time())
        FetchData.start = (FetchData.now - 3600) # last hour
        FetchData.end = FetchData.now
        FetchData.step = '15s' # fetch the data gathered every 15 seconds


    @staticmethod
    def change_setup(self, my_prom_address, my_query, start, step, end):
        self.prom_address = my_prom_address
        self.query = my_query
        self.start = (int(time.time()) - start)
        if end == '':
            self.end = int(time.time())
        else:
            self.end = end
        self.step = step

    @staticmethod
    def fetch_data_range(my_prom_address, my_query, start, step, end, requested, allocatable, with_prediction):
        try:
            response = requests.get('{0}/api/v1/query_range'.format(my_prom_address),
                        params={'query': my_query, 'start':start, 'end':end ,'step':step})

        except requests.exceptions.RequestException as e:
            print(e)
            return

        if response.json()['status'] != "success":
            print("Error processing the request: " + response.json()['status'])
            print("The Error is: " + response.json()['error'])
            return

        results = response.json()['data']['result']

        if (results is None):
            print("the results[] came back empty!")
            return 0, 0, 0

        length = len(results)
        if length > 0:
            print("Saving result in file: {0}".format(FetchData.file_path))

            FetchData.data = pd.DataFrame(data=np.array(results[0]['values']),dtype=float)

            FetchData.data.columns = [u'timestamp', u'value']
            FetchData.write_csv(results) # this is optional


            print FetchData.data["value"].mean(), FetchData.data["value"].std(), FetchData.data["value"].min()

            #curate the data
            if requested > 0:
                FetchData.data.loc[FetchData.data['value'] < ((allocatable - requested)), 'value'] = (allocatable - requested)

            if with_prediction:
                prediction = FetchData.predict_next(FetchData.data)
            else:
                prediction = 0
            #print "after considered requested only:"
            #print FetchData.data["value"].mean(), FetchData.data["value"].std(), FetchData.data["value"].min()

            return FetchData.data["value"].mean(), FetchData.data["value"].std(), FetchData.data["value"].min(),prediction
        else:
            print("the results[] has no entries!")
            return 0, 0, 0


    @staticmethod
    def fetch_data_scalar(my_prom_address, my_query):

        try:
            response = requests.get('{0}/api/v1/query'.format(my_prom_address),
                        params={'query': my_query})

        except requests.exceptions.RequestException as e:
            print(e)
            return

        if response.json()['status'] != "success":
            print("Error processing the request: " + response.json()['status'])
            print("The Error is: " + response.json()['error'])
            return

        results = response.json()['data']['result']

        if (results is None):
            print("the results[] came back empty!")
            return

        length = len(results)
        if length > 0:
            return results[0]['value'][1]
        else:
            print("the results[] has no entries!")
            return



    # Writing to CVS file to make it generic.
    # This allows other ML libraries, e.g. Written in R to open and analyze the date
    @staticmethod
    def write_csv(results):
        with open('mem_usage.csv', 'wb') as csvfile:
            writer = csv.writer(csvfile)
            # Write the header,
            writer.writerow([u'timestamp', u'value'])
            for result in results[0]['values']:
                l = result
                writer.writerow(l)
    @staticmethod
    def chunks(l, n):
        n = max(1, n)
        return (l[i:i+n] for i in range(0, len(l), n))

    @staticmethod
    def predict_next(series):
        # split the file into 24 entries
        data = series.values[:,1:]# filering out time

        #I need to split the values into 100 chunks
        ch = np.array_split(np.array(data),100)

        #make sure the chunks have equal sizes
        lens = np.array([len(i) for i in ch])

        # Mask of valid places in each row
        mask = np.arange(lens.max()) < lens[:,None]

        # Setup output array and put elements from data into masked positions
        out = np.zeros(mask.shape, dtype=data.dtype)
        out[mask] = np.concatenate(data)


        # replace zeros in out[] with median value
        m = np.median(out[out > 0])
        out[out == 0] = m

        arr = []
        for i in range(len(out)):
            arr.append(np.mean(out[i]))#TODO look in the mean usage here?

        #ARIMA
        predicted = None
        history = [float(x) for x in arr]
        try:
            model = ARIMA(history, order=(5,1,0))
            model_fit = model.fit(disp=0)
            output = model_fit.forecast() # forecast does one step forecast
            predicted = output[0]
            print('predicted = %f' % (predicted))
        except Exception as e:
            print ("ARIMA error occured while predicting: ", e)
            return
        #error = mean_squared_error(test, predictions) # needed to optimize the params (future work).
        #print('Test MSE: %.3f' % error)
        return predicted[0]
        #TODO when the prediction does not converge return a safe value

