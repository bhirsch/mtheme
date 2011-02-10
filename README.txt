// $Id$

Module Theme (version 1.x)
----------------------------
The Module Theme module enables you to:
- Theme your website through Drupal's web interface
- Export theme elements with the Features module
- Surface the different theme elements that ship with 
  various modules and themes in one central location 
- Enable modules to ship with a selection of different style 
  sheets, to be easily enabled/disabled for different use-cases by the 
  site administrator or end-user
- Provide an easy interface for enabling, disabling, and copying 
  theme elements provided by other modules and themes
  (Note: It is possible to use the Module Theme module to override
  theming provided by other modules or themes, but this is a hack.
  Use the Subtheme module.)

Installation 
---------------
Place the entire mtheme folder into your modules directory.
Go to Administer -> Site building -> Modules and enable the Module Theme
module. 

To "mtheme-enable" any theme on your site, copy and paste the following 
snippet of code into the bottom of your theme(s) template.php file
(below the last function in the file): 

if (module_exists('mtheme')) {
  mtheme_add_css();  
}

Use 
----------
If you are installing the Module Theme because it is a dependency
of another module in your site, you're done. No additional set-up 
or configuration required. 

If you are a site builder or module developer interested in using
Module Theme to theme your modules (or just add a little CSS here 
or there) read on.

For Site Builders and Module Developers
-----------------------------------------
Go to Administer -> Site building -> Module Theme.

Here you will see a list of all the different CSS selectors included
in your site by modules and themes implementing their theming with 
Module Theme. 

Click the CSS tab to see all these selectors compiled into one single style sheet. 

Click the Add tab to add your own CSS.

To export CSS created with Module Theme go to
Administer -> Site building -> Features -> Create Feature

Under Edit Components select Module Theme, then select the components (CSS selectors)
you want to export. When you're done click Download feature. Now your CSS is 
included in the module you just downloaded.

There are a few different ways to include theming in your custom modules
after exporting as described above:
A. If your custom module is a single feature module, just include the 
   CSS as described above and your done. When the module is enabled,
   all your CSS gets enabled with it.
B. If your custom module is not a feature module, the easiest thing to do 
   is export your CSS as a feature module and include that feature module 
   with your module. (By convention, many modules include a theme directory 
   and put all their theming in there. Consider creating a directory called
   mtheme and storing your theme feature(s) in there.) Then include features 
   in your mtheme directory as dependencies in your .info file.

Maintainer
-------------
Bryan Hirsch, bryan AT bryanhirsch.com
