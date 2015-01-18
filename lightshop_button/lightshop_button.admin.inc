<?php
/**
 * Created by PhpStorm.
 * User: User
 * Date: 15.01.2015
 * Time: 21:32
 */

function lightshop_button_admin_settings($form, &$form_state){
    drupal_add_css(drupal_get_path('module', 'lightshop_button') . '/lightshop_button.admin.css');
    $form = array();
    //типы материалов
    $node_types = node_type_get_types();
    $lightshop_enabled = variable_get('lightshop_node_types', array());
    foreach ($node_types as $ntype => $data) {
        $form['lightshop_button_settings']['ntype']['lbs_' . $ntype] = array(
            '#type' => 'checkbox',
            '#title' => $data->name,
            '#default_value' => variable_get('lbs_' . $ntype, FALSE),
        );
        //отметить можно только те, для которых включен lightshop
        if (!in_array($ntype, $lightshop_enabled)) {
            $form['lightshop_button_settings']['ntype']['lbs_' . $ntype]['#disabled'] = TRUE;
        }
        //коллекции
        $node_fields = field_info_instances('node', $ntype);
        if(count($node_fields) > 0) {
            foreach($node_fields as $nfield) {
                if($nfield['widget']['module'] == 'field_collection') {
                    $form['lightshop_button_settings']['ntype']['lbs_' . $ntype . '_FSETTINGS_' . $nfield['field_name']] = array(
                        '#type' => 'checkbox',
                        '#title' => $nfield['label'],
                        '#attributes' => array('class' => array('light-button-p-left')),
                        '#states' => array(
                            'visible' => array(
                                'input[name="lbs_' . $ntype . '"]' => array('checked' => TRUE),
                            ),
                        ),
                        '#default_value' => variable_get('lbs_' . $ntype . '_FSETTINGS_' . $nfield['field_name'], FALSE),
                    );
                }
            }
        }
    }
    return system_settings_form($form);
}
