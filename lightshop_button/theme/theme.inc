<?php
/**
 * Created by PhpStorm.
 * User: User
 * Date: 17.01.2015
 * Time: 11:10
 */

function theme_lightshop_button_items_table($vars) {
    $header = _lightshop_basket_items_header();
    $rows = $vars['basket']['items'];

    foreach ($rows as $nid => $r) {
        if (isset($rows[$nid]['coll_ids'])) {
            foreach ($rows[$nid]['coll_ids'] as $coll_id => $prod_count) {
                $coll = field_collection_item_load($coll_id);
                $ren = $coll->view('full');
                $ren = render($ren);
                $rows[$nid . '_' . $coll_id] = $r;
                $rows[$nid . '_' . $coll_id]['qty'] = $prod_count;
                $rows[$nid . '_' . $coll_id]['title'] .= $ren;
                unset($rows[$nid . '_' . $coll_id]['coll_ids']);
            }
            unset($rows[$nid]);
        }

    }

    if (isset($vars['basket']['total_sum'])) {
        $currency = isset($vars['basket']['currency']) ? $vars['basket']['currency'] : '';

        foreach ($rows as $nid => $row) {
            $nid_arr = explode('_', $nid);
            if(count($nid_arr) == 2) {
                $price = ($vars['basket']['items'][$nid_arr[0]]['price'] / array_sum($vars['basket']['items'][$nid_arr[0]]['coll_ids'])) * $row['qty'];
                $rows[$nid]['price'] = _lightshop_get_price_numberformat($price, $currency);
            }else {
                $rows[$nid]['price'] = _lightshop_get_price_numberformat($row['price'], $currency);
            }
        }

        $rows[] = array(
            'data' => array(
                array(
                    'data' => t('Order total sum'),
                    'colspan' => count($header) - 1,
                ),
                _lightshop_get_price_numberformat($vars['basket']['total_sum'], $currency),
            ),
        );
    }
    $items_table = array(
        '#theme' => 'fieldset',
        '#title' => t('Order items'),
        '#attached' => array(
            'js' => array(
                'misc/form.js',
                'misc/collapse.js',
            ),
        ),
        '#children' => theme('table', array('header' => $header, 'rows' => $rows)),
    );

    return drupal_render($items_table);
}

/**
 * Preprocess for mail notice template.
 */
function template_preprocess_lightshop_button_mail_notice(&$vars, $hook) {
    $node = $vars['node'];
    $vars['order_items'] = '';
	$prod_type = variable_get('lightshop_node_types', array());
    foreach ($node->order_items['items'] as $nid => $item) {
        $output = '';
        if (isset($item['coll_ids'])) {
			$coll_ids = array_keys($item['coll_ids']);
			foreach ($coll_ids as $coll_id) {
				$output .= l($item['title'], 'node/' . $nid, array('html' => TRUE)) . ":\n";
				$coll_obj_ent = entity_load('field_collection_item', array($coll_id));
				$coll_objects = array_values($coll_obj_ent);
				foreach ($coll_objects[0] as $field_name => $field) {
					if(substr($field_name, 0, 6) == 'field_') {
						$coll_info = field_info_instance('field_collection_item', $field_name, $coll_objects[0]->field_name);
						if((isset($coll_info['label'])) && (isset($field['und'][0]['value']))) {
							$output .= $coll_info['label'] . ':' . $field['und'][0]['value'] . ".\n";
						}
					}
				}
				//количество
				$coll_objects[0]->item_id;//id коллекции
				foreach ($node->order_items['items'] as $one_item_type) {
					foreach ($one_item_type['coll_ids'] as $coll_tmp_id => $coll_count) {
						if ($coll_tmp_id == $coll_objects[0]->item_id) {
							$output .= t('qty: @qty', array('@qty' => $coll_count)) . "\n";
						}
					}
				}
			}
            $output .= l($item['title'], 'node/' . $nid, array('html' => TRUE)) . ' | ' . t('qty: @qty', array('@qty' => array_sum($item['coll_ids'])));
        }
        else if (isset($item['qty'])) {
            $output .= l($item['title'], 'node/' . $nid, array('html' => TRUE)) . ' | ' . t('qty: @qty', array('@qty' => $item['qty']));
        }
        if (isset($item['price'])) {
            $output .= ' | ' . t('price: @price ', array('@price' => _lightshop_get_price_numberformat($item['price'])));
        }
        $vars['order_items'] .= $output . "\n" ;
    }
}

/**
 * Basket block alter.
 */
function template_preprocess_lightshop_button_block_display(&$vars, $hook){
    $basket_items = isset($_SESSION['basket']['items']) ? $_SESSION['basket']['items'] : array();

    $items = array();
    $count_items = 0;
    if ($basket_items) {
        foreach ($basket_items as $nid => $item) {
            $node_title = $item['title'];
            if (isset($item['coll_ids'])) {
                $node_title .= ' (' . array_sum($item['coll_ids']) . ' шт.)';
                $count_items += array_sum($item['coll_ids']);
            }
            else {
                $count_items++;
            }
            $items[] = l($node_title, 'node/' . $nid);
        }
    }

    $vars['count_items'] = $count_items;

    $items_list = array(
        '#theme' => 'fieldset',
        '#title' => t('Items in basket: ') . $count_items,
        '#attached' => array(
            'js' => array(
                'misc/form.js',
                'misc/collapse.js',
            ),
        ),
        '#attributes' => array(
            'class' => array('collapsible', 'collapsed'),
        ),
        '#children' => theme('item_list', array('items' => $items)),
    );

    $vars['basket_items_list'] = $basket_items ? drupal_render($items_list) : '';
}