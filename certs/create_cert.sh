# https://stackoverflow.com/questions/10175812/how-to-create-a-self-signed-certificate-with-openssl

/usr/bin/openssl req -x509 -newkey rsa:4096 -sha256 -days 3650 -nodes \
  -keyout yuneta.key -out yuneta.crt -subj "/CN=yuneta.io" \
  -addext "subjectAltName=DNS:yuneta.io,DNS:yuneta.com,IP:127.0.0.1"
