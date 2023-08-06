#include <iostream>

static const std::string HTML_EXAMPLE_PAGE = R"(
<!DOCTYPE html>
<html>
  <head>
    <meta charset="utf-8">
    <title>Cow</title>
    <style>
      body {
        margin: 0;
        padding: 0;
        background: #000;
        color: #fff;
      }
      header {
      display: flex;
      align-items: center;
      justify-content: center;
      }
      header h1 {
        margin: 0;
        font-size: 2em;
        color: #f00;
      }
    </style>
  </head>
  <body>
   <header>
     <h1>Welcome to Cow!</h1>
   </header>
  </body>
</html>
  )";

static const std::string HTML_EXAMPLE_PAGE_4XX = R"(
<!DOCTYPE html>
<html>
  <head>
    <meta charset="utf-8">
    <title>Cow</title>
    <style>
      body {
        margin: 0;
        padding: 0;
        background: #000;
        color: #fff;
      }
      header {
      display: flex;
      align-items: center;
      } 
      header h1 {
        margin: 0;
        font-size: 2em;
        color: #f00;
      }
    </style>
  </head>
  <body>
   <header>
     <h1>Server Error</h1>
     <a href="/">Go Home</a>
   </header>
  </body>
</html>
  )";
