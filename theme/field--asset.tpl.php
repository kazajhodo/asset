<?php
/**
 * Stripped out all unncessary markup as assets are nested within other areas.
 */
?>
<<?php print $element_type; ?> class="<?php print $classes; ?>"<?php print $attributes; ?>>

<?php if (!$label_hidden): ?>
  <<?php print $element_type; ?> class="field-label"<?php print $title_attributes; ?>><?php print $label ?>:&nbsp;</<?php print $element_type; ?>>
<?php endif; ?>

<?php if(count($items) > 1): ?>
  <<?php print $element_type; ?> class="field-items"<?php print $content_attributes; ?>>
<?php endif; ?>

<?php foreach ($items as $delta => $item): ?>
  <?php if(count($items) > 1): ?>
    <<?php print $element_type; ?> class="field-item <?php print $delta % 2 ? 'odd' : 'even'; ?>"<?php print $item_attributes[$delta]; ?>>
  <?php endif; ?>

    <?php print render($item); ?>

  <?php if(count($items) > 1): ?>
    </<?php print $element_type; ?>>
  <?php endif; ?>
<?php endforeach; ?>

<?php if(count($items) > 1): ?>
  </<?php print $element_type; ?>>
<?php endif; ?>

</<?php print $element_type; ?>>
