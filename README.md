# `Winpache` WAS Development

<img align="center" src="rsc/Logo-Winpache HTTP.png" width="640">
<img align="center" src="rsc/server_client_get_post_api.gif" width="1024">


# `Winpache` project
* HTTP Project VC++.
* HTTPS SSL capable.
* DLL project for site API.

# Download
* [MSI Download : ReleaseWinpache.zip](https://www.dropbox.com/sh/ch86hzxfhbgl7fs/AABdYPPYMteR5SpTsgMEbUeJa?dl=1)
* Extract the zip file.

# Start for simple a `Web Server`(WS)
1. Run `WinpacheSetup.msi`
2. Run the "Start Winpache Project" app that is `StartWinpache.exe`
3. Click the `Start Winpache Server` button, then `Winpache` app will run.
4. In `Winpache`, set the location of homepage files and etc configuration.
5. Start the server, then a simple web server will run.

# To setup the `Main DB` for `Winpache`
* Select the `Database` category in the `ribbon menu`, and complete the `Main DB` setting of `Winpache` by executing up to step 5 in sequence.
* 1. Install the `MariaDB`.
* 2. Install the `ODBC` for `MariaDB`.
* 3. Init ODBC for `Main DB`.
* 4. Create main `database`.
* 5. Create main Talbles and Sample Tables.
2. Once the `main DB` is set, requests, queries and errors that occur in the `Winpache` server are recorded in the DB.
3. The `Main DB` is locally installed as `MariaDB` and accessed by `ODBC` settings, so you can change the `Main DB` by editing the settings.
4. The `Site DB` can be the same as the `Main DB`, and can be separated according to `ODBC` settings.

# Start for a `Web Application Server`(WAS)
1. In `Winpache`, set the web document location and etc configuration.
2. In the `database` menu group, run the `HeidiSQL` and create database and tables.
3. Then, if you click the `Connect to site DB` menu next to the `Restart` menu, the web server is connected to the site server in the site DB.
4. Next, go to the `Project` menu group, go through the numbered menus one by one in order and test the samples and add new `API` functions.
5. Stop and start the server, then a `Web Application Server` will run.

# `Winpache` source
* Only those who study the inside of 'Winpache' need the above source, and the developer who will build the server only needs to download the following Winpache setup files. 
* It also contains the template source to add the API to, and there is a tutorial in the menu.

