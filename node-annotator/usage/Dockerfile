FROM python:2
ADD k8s_analytics.py prom_query.py mem_usage.csv /
RUN pip install  requests pandas numpy kubernetes scipy
RUN pip install -U statsmodels
RUN pip install -U scikit-learn scipy matplotlib
CMD [ "python", "./k8s_analytics.py"]
