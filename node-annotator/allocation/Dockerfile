FROM python:3-alpine
ADD *.py /
RUN pip install kubernetes
CMD [ "python", "./k8s-assessor.py"]
