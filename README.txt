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
1. Place the entire mtheme folder into your modules directory.
   Go to Administer -> Site building -> Modules and enable the Module Theme
   module. 
2. To "mtheme-enable" any theme on your site, copy and paste the following 
   snippet of code into the bottom of your theme(s) template.php file
   (below the last function in the file): 

   if (module_exists('mtheme')) {
     mtheme_add_css();  
   }

Use 
----------
If you are installing the Module Theme module so you can use


For Site Builders and Module Developers
-----------------------------------------
- to enable/disable mymtheme when your module is enabled/disabled, use hook_enable and hook_disable
  

Maintainer
-------------
Bryan Hirsch, bryan AT bryanhirsch.com
