Ubercart Zip2Tax
================

The Zip2Tax module for Ubercart calculates sales tax in selected US states on shipping addresses (or delivery address, for taxed non-shippable products) by performing a lookup of the address from the [Zip2Tax website](https://zip2tax.com). You must have a valid Zip2Tax account.

This module also provides a lookup page for sales tax, at Admininstration > Store > Reports > Zip2Tax lookup (admin/store/reports/zip2tax-lookup).

Zip2Tax has indicated that they might introduce an API for Canada at some point; when that is done, support will be added in this module.

Installation
------------

Install this module using [the official Backdrop CMS instructions](https://backdropcms.org/guide/modules).

Go to the configuration page at Administration > Store > Configuration > Taxes > Zip2Tax (admin/store/settings/taxes/uc_zip2tax). Expand each section to make your settings (explanation follows), then select "Save configuration".

### Tax Lookup credentials

Enter your Zip2Tax username and password.

### Lookup status

Check the boxes for the product statuses for which you want to do lookup from the Zip2Tax website. Typically you will check all but "Payment received" and "Completed". If you check the last two, then when you pull up an old order, Ubercart will attempt to look up the tax rate all over again, and usually, the desired behavior is that once an order has been paid for and completed, you don't want tax lookup to keep happening; the result might change if the tax rates have changed since the order was placed. However, if you edit orders after completion and want to recalculate the tax, you can temporarily check these boxes as well.

### Tax ID

Enter an integer that will be used to identify Ubercart Zip2Tax's tax line items. This must be different from the `tax_id` used by any other sales tax module you might be using in combination with this one. If Ubercart Zip2Tax is your only sales tax module, then you can use the default value.

### Display

These settings control what information is shown to customers on the tax line item.

* Show state — displays the state for which tax was calculated.

* Show tax jurisdiction — displays any jurisdiction codes returned by Zip2Tax for the jurisdiction. Note that this only affects the name that is shown to the customer. The jurisdiction code that shows up in the Sales Tax Report is constructed automatically from the values returned by the Zip2Tax lookup.

* Show tax rate — display the tax rate used. 

* Tax-exempt roles — This lists all the roles on your site. Check any role that should be tax-exempt.

* Taxation zones — This lists all states. Check the states that are nexus states for your store. Ubercart Zip2Tax will look up tax rates from the Zip2Tax website only for these states.

    Once you have selected one or more states as nexus states and saved your changes by clicking "Save configuration", you will see additional sections of the form "Taxables in XX" for each of your selected states.

* Taxation method — You must choose one of two methods for specifying whether a product is taxable. You can do this by creating two different product types (e.g., "taxable product" and "nontaxable product", or (and this is the recommended way) by creating a taxonomy specifically to indicate taxation status. You can create a taxonomy term for each distinct taxable scenario, and then assign the appropriate term(s) to each of your products.

Within each taxable state, make your choices:

* Taxed product types — specifies which of your Ubercart product types to apply sales tax in this state, OR

* Taxed [vocabulary] terms — specifies the vocabulary terms for products that are taxable in this state.

* Taxed line items — specifies taxability of additional line items (e.g., shipping) in this state.

* Default rate — If for some reason, Zip2Tax cannot look up the tax rate (usually due to a mistyped or incomplete address), this lets you choose a fallback rate for the state. If you leave this zero, no tax will be calculated, but the transaction will be permitted to go through.

Issues
------

Bugs and feature requests should be reported in [the issue queue](https://github.com/backdrop-contrib/uc_zip2tax/issues).

No Warranties
-------------

This module is provided entirely as-is. You are welcome to modify it as you wish (subject to the LICENSE.txt). The author makes no warranties of its accuracy or compliance with the tax laws in any jurisdiction and makes no warranties of the accuracy of the data provided by Zip2Tax. The author is not affiliated in any way with Zip2Tax.

Current Maintainers
-------------------

- [Robert J. Lang](https://github.com/bugfolder)

Credits
-------

- Ported to Backdrop CMS by [Robert J. Lang](https://github.com/bugfolder).
- Originally written for Drupal by [Robert J. Lang](https://github.com/bugfolder) for OrigamiUSA.

License
-------

This project is GPL v2 software.
See the LICENSE.txt file in this directory for complete text.

