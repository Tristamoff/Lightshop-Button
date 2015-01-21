/**
 * Created by User on 20.01.2015.
 */
(function($) {
    Drupal.behaviors.lightshop_button = {
        attach: function(context, settings) {
            var inputCountValue = $("input[name=count]", context);
            inputCountValue.bind({
                keypress: function(e) {
                    if (e.which != 13 && e.which != 8 && e.which !== 0 && (e.which < 48 || e.which > 57)) {
                        return false;
                    }
                }
            });
        }
    };
})(jQuery);