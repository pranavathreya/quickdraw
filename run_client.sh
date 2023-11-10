SERVER="localhost"
if [ -z $1 ]; then 
    echo "defaulting to localhost"; 
else 
    echo "var is set to '$var'";
    SERVER=$1
fi

./client.o $SERVER 5002 5001 fuck 2> logs/client.log 

