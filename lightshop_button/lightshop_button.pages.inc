<?php
/**
 * Created by PhpStorm.
 * User: User
 * Date: 16.01.2015
 * Time: 21:08
 */

function lightshop_button_add2basket($nid, $coll_id, $type) {
    $node = node_load($nid);
    // Verification of the absence of this node in basket.
    if (_lightshop_button_in_basket($node->nid, $coll_id)) {
        drupal_set_message(t('"@title" is already in your basket', array('@title' => $node->title)
        ), 'warning');
    }
    else {
        global $user;
        if ($user->uid == 0 && !drupal_session_started()) {
            // Must initialize sessions for anonymous users.
            drupal_session_start();
        }
        $item = array();
        $item['coll_ids'] = array((int) $coll_id => 1);
        $item['title'] = $node->title;
        if (variable_get('lightshop_show_qty', TRUE)) {
            $item['qty'] = 1;
        }
        if (variable_get('lightshop_show_price', FALSE) && ($price = _lightshop_get_price($node))) {
            if ($price <= 0 || !is_numeric($price)
            ) {
                drupal_set_message(t('Sorry, but this product currently unavailable for ordering'), 'warning');
                $item = FALSE;
            }
            else {
                if (!isset($_SESSION['basket']['currency'])) {
                    $_SESSION['basket']['currency'] = variable_get('lightshop_price_suffix', '');
                }
                if (!isset($_SESSION['basket']['total_sum'])) {
                    $_SESSION['basket']['total_sum'] = 0;
                }
                $_SESSION['basket']['total_sum'] += $price;
                $item['price'] = $_SESSION['basket']['store'][$node->nid]['unit_price'] = $price;
            }
        }
        if ($item) {
            if(!$_SESSION['basket']['items'][$node->nid]) {
                $_SESSION['basket']['items'][$node->nid] = $item;
            }
            else {
                $_SESSION['basket']['items'][$node->nid]['coll_ids'][(int) $coll_id] = 1;
                $_SESSION['basket']['items'][$node->nid]['price'] += $price;
            }
            drupal_set_message(t("'@title' added in your basket", array('@title' => $node->title)));
        }
    }
    if ($type == 'ajax') {
        $basket_block = lightshop_block_content();
        $commands = array();
        $commands[] = ajax_command_replace(NULL, lightshop_button_add2basket_link($node->nid, $coll_id));
        $commands[] = ajax_command_replace('#lightshop-ajax-block-content', render($basket_block));
        $output = array('#type' => 'ajax', '#commands' => $commands);
        ajax_deliver($output);
    }
    else {
        drupal_goto();
    }
}