#!/bin/bash

for f in /$2/*.in ; do
	$1 < ${f%in}in 1> ${f%in}out2 2> ${f%in}err2;

	if diff ${f%in}err2 ${f%in}err > /dev/null 2>&1
	then
		echo ${f%in}err2 ${f%in}err" equal files"
	else
		echo ${f%in}err2 ${f%in}err" different files"
	fi

	diff ${f%in}out2 ${f%in}out;

	rm ${f%in}err2;

	rm ${f%in}out2;
done
