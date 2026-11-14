# Configuring an SSL certificate on Nginx

To serve a website over HTTPS you need a certificate signed by a trusted authority and the matching private key. The easiest free option is Let's Encrypt, which issues certificates valid for ninety days and can renew them automatically.

Install the certbot tool from your distribution packages, then run it with the nginx plugin and the domain name of your site. Certbot proves that you control the domain by placing a temporary file on the web server, requests the certificate and edits the Nginx configuration for you.

If you prefer to configure it by hand, add a server block that listens on port 443 and set the ssl_certificate directive to the full chain file and ssl_certificate_key to the private key. Keep the private key readable only by root. It is also good practice to redirect all plain HTTP traffic on port 80 to HTTPS with a permanent 301 redirect.

After every change run nginx -t to check the syntax and then reload the service. Remember that certificates expire: a systemd timer or a cron job running certbot renew twice a day avoids the classic outage where the site suddenly shows a security warning to every visitor.
