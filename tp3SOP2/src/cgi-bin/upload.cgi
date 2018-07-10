#!/usr/bin/perl -wT
# TP3. Sistemas Operativos 2.
# Lopez Gaston.
# Archivo upload.cgi 
# El presente codigo es una adaptacion del codigo extraido de:
# https://www.sitepoint.com/uploading-files-cgi-perl-2/
use strict; #Script seguro
use CGI; #Incluye PERL CGI
use CGI::Carp qw ( fatalsToBrowser ); #Muestra errores en pagina web
use File::Basename; #Modulo a usar en el script

$CGI::POST_MAX = 1024 * 5000; #Limite de 5MB de subida.
my $safe_filename_characters = "a-zA-Z0-9_.-"; #Caracteres seguros para nombres de archivos.
my $upload_dir = "/var/www/html/Driver"; #Directorio en el servidor para almacenar el archivo.

my $query = new CGI; #Creacion de objeto CGI
my $filename = $query->param("cargar_modulo"); #Se lee lo que se introduce en el formulario.

#Test si el archivo esta vacio. (Puede ser problema de tamanio).
if ( !$filename )
{
print $query->header ( );
print "Error al subir el archivo.";
exit;
}

#Seguridad

#Parseo y verificacion de que extension sea un . seguido de 0 o mas caracteres.
my ( $name, $path, $extension ) = fileparse ( $filename, '..*' );
$filename = $name . $extension; #Sin path
$filename =~ tr/ /_/; #Cambio espacios por guion bajo
$filename =~ s/[^$safe_filename_characters]//g; #Verifico expresiones regulares

#Verifico match de expresiones regulares con el filename.
if ( $filename =~ /^([$safe_filename_characters]+)$/ )
{
$filename = $1;
}
else
{
die "Filename contains invalid characters";
}

my $upload_filehandle = $query->upload("cargar_modulo"); #Seteo el file handle al archivo

open ( UPLOADFILE, ">$upload_dir/$filename" ) or die "$!"; #$! es similar errno.
binmode UPLOADFILE; #Archivo binario.

while ( <$upload_filehandle> )
{
print UPLOADFILE;
}

close UPLOADFILE;


print $query->header ( );
print <<END_HTML;
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<title>TP3 Lopez Gaston</title>
</head>
<body>
<center>
</br>
<h1>Trabajo Practico 3 de Sistemas Operativos II</h1>
</br>
<h2>Operaciones sobre modulos</h2>
</br>
<p>El modulo fue subido y almacenado en el servidor con exito.</p>
</br>
<form action="/cgi-bin/insertar_file.cgi" method="get">
  <input name="modulo" hidden value="$filename">
  <button style="FONT-SIZE: 13pt">Insertar modulo</button>
</form>
</br></br>
<a href="../index.lighttpd.html"><button style="FONT-SIZE: 13pt"><p>Volver a la Pagina Principal</button></a></p>
</center>
</body>
</html>
END_HTML
