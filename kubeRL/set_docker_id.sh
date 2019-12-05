#!/bin/bash

export $(cat config.env)
echo "The USERID=$USERID"
if [ -z "$USERID" ]
then
	userpath=""
else
	userpath=$USERID"\/"
fi

templates=./manifests/*.template
for f in $templates
do
	echo "Updating docker_id in $f"
	yaml="."`echo $f| cut -d'.' -f2`".yaml"
	echo "Updating docker_id to file $yaml"
	sedcmd="s/_USERID_/$userpath/g"
	sed $sedcmd $f > $yaml
done
