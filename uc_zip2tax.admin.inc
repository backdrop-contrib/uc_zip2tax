<?php
/**
 * @file
 * Zip2Tax module for Ubercart, admin page.
 */

/**
 * Admin settings menu callback.
 */
function uc_zip2tax_settings_form($form, &$form_state) {
  $config = config('uc_zip2tax.settings');
  $form['#config'] = 'uc_zip2tax.settings';

  // Tax ID for this module

  $form['tax_id'] = array(
    '#type' => 'fieldset',
    '#title' => t('Tax ID'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['tax_id']['uc_zip2tax_tax_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Tax ID'),
    '#description' => t('Enter a unique integer that will be used internally to identify taxes created by uc_zip2tax. This allows you to combine uc_zip2tax with other taxable modules, if needed, as long as those other modules use different IDs.'),
    '#default_value' => $config->get('uc_zip2tax_tax_id'),
  );

  // Zip2Tax lookup settings

  $form['lookup'] = array(
    '#type' => 'fieldset',
    '#title' => t('Tax lookup credentials'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['lookup']['uc_zip2tax_user'] = array(
    '#type' => 'textfield',
    '#title' => t('User name'),
    '#description' => t('Enter your Zip2Tax user name.'),
    '#default_value' => $config->get('uc_zip2tax_user'),
  );
  $form['lookup']['uc_zip2tax_pass'] = array(
    '#type' => 'textfield',
    '#title' => t('Password'),
    '#description' => t('Enter your Zip2Tax password.'),
    '#default_value' => $config->get('uc_zip2tax_pass'),
  );

  // Mutable status settings

  $form['mutable_status'] = array(
    '#type' => 'fieldset',
    '#title' => t('Mutable status'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['mutable_status']['uc_zip2tax_lookup_statuses'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Mutable statuses'),
    '#description' => t('For unchecked statuses, use the previously calculated tax value that is stored in the order. For checked statuses, dynamically update the sales tax based on current data in the order.'),
    '#multiple' => TRUE,
    '#options' => uc_order_status_options_list(),
    '#default_value' => $config->get('uc_zip2tax_lookup_statuses'),
  );

  // Display settings

  $form['display'] = array(
    '#type' => 'fieldset',
    '#title' => t('Display'),
    '#description' => t('Configure what information is shown on the tax line item.'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['display']['uc_zip2tax_show_state'] = array(
    '#title' => t('Show state'),
    '#description' => t('Set to Yes if you want to show the state on the tax line item, so you and the customer can see what state tax was used.'),
    '#type' => 'radios',
    '#options' => array(
      '0' => t('No'),
      '1' => t('Yes'),
    ),
    '#default_value' => $config->get('uc_zip2tax_show_state'),
  );

  $form['display']['uc_zip2tax_show_jurisdiction'] = array(
    '#title' => t('Show tax jurisdiction'),
    '#description' => t('Set to Yes if you want to show the jurisdiction on the tax line item.'),
    '#type' => 'radios',
    '#options' => array(
      '0' => t('No'),
      '1' => t('Yes'),
    ),
    '#default_value' => $config->get('uc_zip2tax_show_jurisdiction'),
  );

  $form['display']['uc_zip2tax_show_rate'] = array(
    '#title' => t('Show tax rate'),
    '#description' => t('Set to Yes if you want to show the rate on the tax line item.'),
    '#type' => 'radios',
    '#options' => array(
      '0' => t('No'),
      '1' => t('Yes'),
    ),
    '#default_value' => $config->get('uc_zip2tax_show_rate'),
  );

  // Wholesale users - exclude users with these role from tax.

  $form['roles'] = array(
    '#type' => 'fieldset',
    '#title' => t('Tax-exempt roles'),
    '#description' => t('Exclude tax from any user who has one of the checked roles.'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['roles']['uc_zip2tax_wholesale_roles'] = array(
    '#type' => 'checkboxes',
    '#multiple' => TRUE,
    '#options' => user_roles(TRUE),
    '#default_value' => $config->get('uc_zip2tax_wholesale_roles'),
  );

  // Taxation settings

  $product_types = uc_product_type_names();
  unset($product_types['product_kit']);

  $line_items = array();
  foreach (_uc_line_item_list() as $line_item) {
    if (!in_array($line_item['id'], array('generic', 'tax', 'tax_display', 'subtotal', 'tax_subtotal', 'total'))) {
      $line_items[$line_item['id']] = $line_item['title'];
    }
  }

  // Taxation group that gets updated by ajax. Everything below here can be
  // updated via AJAX.

  $form['taxation'] = array(
    '#type' => 'fieldset',
    '#title' => t('Taxation'),
    '#description' => '',
    '#prefix' => '<div id="taxation">',
    '#suffix' => '</div>',
  );

  // Nexus zones - these are the states/provinces where we charge sales tax

  $form['taxation']['taxation_zones'] = array(
    '#type' => 'fieldset',
    '#title' => t('Taxation zones'),
    '#description' => t('Choose where you are going to collect sales tax.'),
  );

  $all_countries = uc_zip2tax_country_option_list();
  $nexus_countries = isset($form_state['values']['uc_zip2tax_nexus_countries']) ? $form_state['values']['uc_zip2tax_nexus_countries'] : $config->get('uc_zip2tax_nexus_countries');
  $form['taxation']['taxation_zones']['uc_zip2tax_nexus_countries'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Nexus countries'),
    '#description' => t('Collect sales tax if user is from one of these countries.'),
    '#multiple' => TRUE,
    '#options' => $all_countries,
    '#default_value' => $nexus_countries,
    '#ajax' => array(
      'callback' => 'uc_zip2tax_taxation_callback',
      'wrapper' => 'taxation',
      'method' => 'replace',
      'effect' => 'fade',
    ),
  );

  $all_zones = array();
  foreach ($nexus_countries as $key => $value) {
    if ($value) {
      $all_zones += uc_zone_option_list()[$all_countries[$key]];
    }
  }
  ksort($all_zones);
  $nexus_zones = isset($form_state['values']['uc_zip2tax_nexus_zones']) ? $form_state['values']['uc_zip2tax_nexus_zones'] : $config->get('uc_zip2tax_nexus_zones');
  $form['taxation']['taxation_zones']['uc_zip2tax_nexus_zones'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Nexus zones'),
    '#description' => t('Collect sales tax if user is from one of these states/provinces.'),
    '#multiple' => TRUE,
    '#options' => $all_zones,
    '#default_value' => $nexus_zones,
    '#ajax' => array(
      'callback' => 'uc_zip2tax_taxation_callback',
      'wrapper' => 'taxation',
      'method' => 'replace',
      'effect' => 'fade',
    ),
  );

  // Taxation method -- whether to use product type or a taxonomy term

  $form['taxation']['taxation_method'] = array(
    '#type' => 'fieldset',
    '#title' => t('Taxation method'),
    '#description' => t('Choose how to distinguish taxable and nontaxable products. If by product type, you will select the taxable product types within each nexus zone. If by taxonomy term, select the taxable vocabulary here, then select terms that identify taxable products within each nexus zone.'),
  );
  $method_options = array(
    UC_ZIP2TAX_METHOD_PRODUCT_TYPE => t('Product Type'),
    UC_ZIP2TAX_METHOD_TAXONOMY_TERM => t('Taxonomy Term'),
  );
  $method = isset($form_state['values']['uc_zip2tax_taxation_method']) ? $form_state['values']['uc_zip2tax_taxation_method'] : $config->get('uc_zip2tax_taxation_method');
  $form['taxation']['taxation_method']['uc_zip2tax_taxation_method'] = array(
    '#type' => 'radios',
    '#title' => t('Method'),
    '#options' => $method_options,
    '#default_value' => $method,
    '#ajax' => array(
      'callback' => 'uc_zip2tax_taxation_callback',
      'wrapper' => 'taxation',
      'method' => 'replace',
      'effect' => 'fade',
    ),
  );

  if ($method == UC_ZIP2TAX_METHOD_TAXONOMY_TERM) {
    $taxation_vocabularies = array(0 => t('—None—'));
    // $vocabularies = taxonomy_vocabulary_get_names();
    $vocabularies = taxonomy_vocabulary_load_multiple(FALSE);
    foreach ($vocabularies as $machine_name => $vocabulary) {
      $taxation_vocabularies[$vocabulary->machine_name] = $vocabulary->name;
    }
    $taxation_vocabulary = isset($form_state['values']['uc_zip2tax_taxation_vocabulary']) ? $form_state['values']['uc_zip2tax_taxation_vocabulary'] : $config->get('uc_zip2tax_taxation_vocabulary');
    $form['taxation']['taxation_method']['uc_zip2tax_taxation_vocabulary'] = array(
      '#type' => 'select',
      '#title' => t('Taxable Vocabulary'),
      '#options' => $taxation_vocabularies,
      '#default_value' => $taxation_vocabulary,
      '#disabled' => $method == UC_ZIP2TAX_METHOD_PRODUCT_TYPE,
      '#ajax' => array(
        'callback' => 'uc_zip2tax_taxation_callback',
        'wrapper' => 'taxation',
        'method' => 'replace',
        'effect' => 'fade',
      ),
    );
    $taxation_term_names = db_query('
      SELECT tid, name
      FROM {taxonomy_term_data}
      WHERE vocabulary = :vocabulary
      ', array(':vocabulary' => $taxation_vocabulary))
      ->fetchAllKeyed();
  }

  // Tax details for each zone

  $nexus_zones_used = array();
  foreach ($nexus_zones as $key => $value) {
    if ($value) {
      $nexus_zones_used[$key] = $value;
    }
  }
  foreach ($nexus_zones_used as $zone) {
    $state = uc_get_zone_code($zone);
    $fieldset_name = "taxation_{$state}";
    if ($method == UC_ZIP2TAX_METHOD_PRODUCT_TYPE) {
      $description = t('Select product types and line items subject to @state state sales tax.', array('@state' => $state));
    }
    else {
      $description = t('Select taxonomy terms and line items subject to @state state sales tax.', array('@state' => $state));
    }
    $form['taxation'][$fieldset_name] = array(
      '#type' => 'fieldset',
      '#title' => t('Taxables in @state', array('@state' => $state)),
      '#description' => $description,
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
    );
    if ($method == UC_ZIP2TAX_METHOD_PRODUCT_TYPE) {
      $state_product_types_var = "uc_zip2tax_{$state}_product_types";
      $state_product_types = isset($form_state['values'][$state_product_types_var]) ? $form_state['values'][$state_product_types_var] : $config->get($state_product_types_var);
      $form['taxation'][$fieldset_name][$state_product_types_var] = array(
        '#type' => 'checkboxes',
        '#title' => t('Taxed product types'),
        '#multiple' => TRUE,
        '#options' => $product_types,
        '#default_value' => $state_product_types,
      );
    }
    else { // $method == UC_ZIP2TAX_METHOD_TAXONOMY_TERM
      $state_terms_var = "uc_zip2tax_{$state}_terms";
      $state_terms = isset($form_state['values'][$state_terms_var]) ? $form_state['values'][$state_terms_var] : $config->get($state_terms_var);
      $form['taxation'][$fieldset_name][$state_terms_var] = array(
        '#type' => 'checkboxes',
        '#title' => t('Taxed @vocabulary terms', array('@vocabulary' => $vocabularies[$taxation_vocabulary])),
        '#multiple' => TRUE,
        '#options' => $taxation_term_names,
        '#default_value' => $state_terms,
      );
    }
    $state_line_items_var = "uc_zip2tax_{$state}_line_items";
    $state_line_items = isset($form_state['values'][$state_line_items_var]) ? $form_state['values'][$state_line_items_var] : $config->get($state_line_items_var);
    $form['taxation'][$fieldset_name][$state_line_items_var] = array(
      '#type' => 'checkboxes',
      '#title' => t('Taxed line items'),
      '#multiple' => TRUE,
      '#options' => $line_items,
      '#default_value' => $state_line_items,
    );
    $state_default_var = "uc_zip2tax_{$state}_default_rate";
    $state_default = isset($form_state['values'][$state_default_var]) ? $form_state['values'][$state_default_var] : $config->get($state_default_var);
    $form['taxation'][$fieldset_name][$state_default_var] = array(
      '#type' => 'textfield',
      '#title' => t('Default rate'),
      '#description' => t('Enter the default rate, if any, to use if lookup fails.'),
      '#default_value' => $state_default,
    );
  }

  return system_settings_form($form);
}

/**
 * Ajax callback to return all the taxation settings.
 */
function uc_zip2tax_taxation_callback($form, &$form_state) {
  return $form['taxation'];
}

/**
 * Returns form to look up the tax rate for a specified address.
 */
function uc_zip2tax_lookup_form($form, &$form_state) {

  $form[] = array(
    '#markup' => t('Look up the tax rate for an address.'),
    '#prefix' => '<p>',
    '#suffix' => '</p>',
  );
  $form['street1'] = array(
    '#type' => 'textfield',
    '#title' => t('Street address 1'),
  );
  $form['street2'] = array(
    '#type' => 'textfield',
    '#title' => t('Street address 2'),
  );
  $form['city'] = array(
    '#type' => 'textfield',
    '#title' => t('City'),
  );
  $form['country'] = array(
    '#type' => 'select',
    '#title' => t('Country'),
    '#options' => uc_zip2tax_country_option_list(),
    '#default_value' => isset($form_state['values']['country']) ? $form_state['values']['country'] : 840,
    '#ajax' => array(
      'callback' => 'uc_zip2tax_lookup_form_zone_callback',
      'wrapper' => 'tax-lookup-zone',
      'method' => 'replace',
      'effect' => 'fade',
    ),
  );

  if (isset($form_state['values']['country'])) {
    $key = $form_state['values']['country'];
  }
  else {
    $key = 840;
  }
  $zones = uc_zone_option_list()[uc_country_option_list()[$key]];
  $form['zone'] = array(
    '#type' => 'select',
    '#title' => t('State/Province'),
    '#options' => $zones,
    '#prefix' => '<div id="tax-lookup-zone">',
    '#suffix' => '</div>',
  );
  $form['zip'] = array(
    '#type' => 'textfield',
    '#title' => t('Postal Code'),
  );
  $form['tax_info'] = array(
    '#type' => 'fieldset',
    '#title' => t('Tax info'),
    '#description' => '',
    '#prefix' => '<div id="tax-lookup-info">',
    '#suffix' => '</div>',
  );

  $form['submit'] = array(
    '#type' => 'button',
    '#value' => t('Lookup'),
    '#attributes' => array('class' => array('tax-lookup')),
    '#ajax' => array(
      'callback' => 'uc_zip2tax_lookup_form_info_callback',
      'wrapper' => 'tax-lookup-info',
      'method' => 'replace',
      'effect' => 'fade',
    ),
  );
  return $form;
}

/**
 * Ajax callback to set the list of zones based on country choice.
 */
function uc_zip2tax_lookup_form_zone_callback($form, &$form_state) {
  return $form['zone'];
}

/**
 * Ajax callback to look up a tax rate on the interactive form.
 */
function uc_zip2tax_lookup_form_info_callback($form, &$form_state) {
  $street1 = $form_state['values']['street1'];
  $street2 = $form_state['values']['street2'];
  $city = $form_state['values']['city'];
  $usa = uc_country_option_list()[840];
  $zones = uc_zone_option_list()[$usa];
  $zone = $zones[$form_state['values']['zone']];
  $zip = $form_state['values']['zip'];

  $lookup = _uc_zip2tax_lookup($street1, $street2, $city, $state, $zip);
  if ($lookup['fail']) {
    $result = t('Lookup failed.');
  }
  else {
    $result = t('<ul><li>Rate: @rate%</li><li>Code: @code</li></ul>',
      array('@rate' => $lookup['rate'], '@code' => $lookup['code']));
  }
  $form_piece = $form['tax_info'];
  $form_piece['#description'] = $result;
  return $form_piece;
}

/**
 * Returns the keyed list of all supported countries.
 */
function uc_zip2tax_country_option_list() {
  $supported_countries = array(
    "United States",
//    "Canada", // @ todo: uncomment this when Canada becomes available.
  );
  return array_intersect(uc_country_option_list(), $supported_countries);
}

/**
 * Returns the keyed list of all supported zones.
 */
function uc_zip2tax_zone_option_list() {
  $zones = array();
  foreach (uc_zip2tax_country_option_list() as $key => $country) {
    $zones += uc_zone_option_list()[$country];
  }
  return $zones;
}
