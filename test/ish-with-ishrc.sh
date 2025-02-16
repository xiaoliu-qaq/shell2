#!/bin/bash
/bin/rm -f ~/.ishrc
/bin/cp ishrc.ish ~/.ishrc
$1
/bin/rm -f ~/.ishrc
