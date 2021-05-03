#include <errno.h>
#include <unistd.h>
#include <malloc.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include "openssl/ssl.h"
#include "openssl/err.h"
#include "sslLib.h"
#include "utility.h"

SSL_CTX *g_ctx = NULL;

static bool isRoot();

static SSL_CTX* InitServerCTX(void);

static STATUS LoadCertificates(SSL_CTX* ctx, const char* CertFile, const char* KeyFile);

// Initialize SSL library, create a the SSL_CTX, and load certificates.
STATUS initCTX()
{
    SSL_CTX* ctx;

    if (g_ctx != NULL)
    {
        return (STATUS_ERROR);
    }

    if(!isRoot())
    {
        logError("This program must be run as root/sudo user!!");
        return (STATUS_ERROR);
    }

    SSL_library_init();

    ctx = InitServerCTX();        /* initialize SSL */
    if ( ctx == NULL )
    {
        ERR_print_errors_fp(stderr);
        return (STATUS_ERROR);
    }

    /* load certs */
    if (LoadCertificates(ctx, "mycert.pem", "mycert.pem") == STATUS_OK)
    {
        g_ctx = ctx;
        return (STATUS_OK);
    }
    else if (LoadCertificates(ctx, "/usr/bin/mycert.pem", "/usr/bin/mycert.pem") == STATUS_OK)
    {
        g_ctx = ctx;
        return (STATUS_OK);
    }
    else
    {
        SSL_CTX_free(ctx);         /* release context */
        return (STATUS_ERROR);
    }
}

/* To run SSL, we need to login as user: root */
bool isRoot()
{
    if (getuid() != 0)
    {
        return false;
    }
    else
    {
        return true;
    }
}

/* Initialize a SSL_CTX */
SSL_CTX* InitServerCTX(void)
{   
    SSL_METHOD *method;
    SSL_CTX *ctx;

    OpenSSL_add_all_algorithms();  /* load & register all cryptos, etc. */
    SSL_load_error_strings();   /* load all error messages */
    method = (SSL_METHOD *)TLSv1_2_server_method();  /* create new server-method instance */
    ctx = SSL_CTX_new(method);   /* create new context from method */

    return ctx;
}

/* Load Certificates for SSL_CTX */
STATUS LoadCertificates(SSL_CTX* ctx, const char* CertFile, const char* KeyFile)
{
    /* set the local certificate from CertFile */
    if ( SSL_CTX_use_certificate_file(ctx, CertFile, SSL_FILETYPE_PEM) <= 0 )
    {
        ERR_print_errors_fp(stderr);
        return (STATUS_ERROR);
    }
    /* set the private key from KeyFile (may be the same as CertFile) */
    if ( SSL_CTX_use_PrivateKey_file(ctx, KeyFile, SSL_FILETYPE_PEM) <= 0 )
    {
        ERR_print_errors_fp(stderr);
        return (STATUS_ERROR);
    }
    /* verify private key */
    if ( !SSL_CTX_check_private_key(ctx) )
    {
        fprintf(stderr, "Private key does not match the public certificate\n");
        return (STATUS_ERROR);
    }

    return (STATUS_OK);
}

/* Show Certificates for SSL_CTX */
void showCerts(SSL* ssl)
{   
    X509 *cert;
    char *line;

    cert = SSL_get_peer_certificate(ssl); /* Get certificates (if available) */
    if ( cert != NULL )
    {
        logDebug("Server certificates:");
        line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        logDebug("Subject: %s", line);
        free(line);
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        logDebug("Issuer: %s", line);
        free(line);
        X509_free(cert);
    }
    else
        logDebug("No certificates.");
}

/* Close the SSL_CTX */
void exitCTX(void)
{
    if (g_ctx != NULL)
    {
        SSL_CTX_free(g_ctx);         /* release context */
    }
    g_ctx = NULL;
}
