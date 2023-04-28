#!/usr/bin/perl

use strict;
use warnings;
use CGI qw(:standard);

print header(-type => 'text/html');
print start_html(-title => 'CGI Response Perl',
                 -style => {'src' => 'https://cdn.jsdelivr.net/npm/bootstrap@5.3.0-alpha3/dist/css/bootstrap.min.css'},
                 -head  => meta({'charset' => 'UTF-8'}));
print '<nav class="navbar navbar-expand navbar-dark bg-dark">';
print '<a class="navbar-brand" href="#">webserv42</a>';
print '<ul class="navbar-nav">';
print '<li class="nav-item">';
print '<a class="nav-link" href="../index.html">Accueil</a>';
print '</li>';
print '<li class="nav-item">';
print '<a class="nav-link" href="../televerser.html">Téléverser</a>';
print '</li>';
print '<li class="nav-item">';
print '<a class="nav-link" href="../team.html">Notre équipe</a>';
print '</li>';
print '<li class="nav-item">';
print '<a class="nav-link" href="../cgi.html">CGI</a>';
print '</li>';
print '</ul>';
print '</nav>';
print start_body();
print "<h1>Test CGI with Perl</h1>";
print "<p>Hello World!</p>";

my $query = CGI->new;

if (defined $query->param('name')) {
    print "<p>Your name is: " . $query->param('name') . "</p>";
}

if (defined $query->param('login')) {
    print "<p>Your login is: " . $query->param('login') . "</p>";
}

print "<h3>Environments variables:</h3>";
foreach my $key (sort keys %ENV) {
    print "<li>$key: $ENV{$key}</li>";
}

print end_html();